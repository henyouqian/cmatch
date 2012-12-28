#include "cm_account.h"
#include "cm_thread.h"
#include "cm_util.h"
#include <postgresql/libpq-fe.h>
#include <uuid/uuid.h>

static const size_t USERNAME_MAX = 40;
static const size_t PASSWORD_MAX = 40;
static const size_t UNESC_BUF_MAX = 100;
static const time_t SESSION_LIFE_SEC = 60*60;

static inline void _send_error(int err, evhtp_request_t* req)
{
    evbuffer_add_printf(req->buffer_out, "{error=%d}", err);
    evhtp_send_reply(req, EVHTP_RES_BADREQ);
}

//register====================================================
static inline void __register_clean(PGresult *res, char *pw64)
{
    if (res)
        PQclear(res);
    free(pw64);
}

void cm_register_account(evhtp_request_t *req, void *arg)
{
    //variables
    const int err_param = 1;
    const int err_user_exist = 2;
    
    PGresult *res = NULL;
    char *pw64 = NULL;
    
    #define _register_clean() __register_clean(res, pw64)
    
    //parse param
    int err = 0;
    const char *username = kvs_find_string(&err, req->uri->query, "username");
    const char *password = kvs_find_string(&err, req->uri->query, "password");
    if (err) {
        _send_error(err_param, req);
        return _register_clean();
    }
    size_t username_len = strlen(username);
    size_t password_len = strlen(password);
    if (username_len == 0 || password_len == 0
            || username_len > UNESC_BUF_MAX || password_len > PASSWORD_MAX) {
        _send_error(err_param, req);
        return _register_clean();
    }
    
    //unescape username
    char buf[UNESC_BUF_MAX+1] = {0};
    char *ue_username = buf;
    evhtp_unescape_string((unsigned char**)&ue_username, (unsigned char*)username, username_len+1); //include \0
    username_len = strlen(ue_username);
    if (username_len > USERNAME_MAX) {
        _send_error(err_param, req);
        return _register_clean();
    }
    
    //encrypt password
    char hash[20];
    sha1(hash, password, strlen(password));
    pw64 = base64_cf(hash, 20); //free: free(password_b64);
    
    //db
    thread_ctx* pctx = cm_get_thread_ctx();
    PGconn *acdb = pctx->accountdb;
    const char *vars[2] = {
        ue_username,
        pw64,
    };

    res = PQexecParams(acdb,  //free: PQclear(res);
                                 "INSERT INTO user_account (username, password) VALUES($1, $2)",
                                 2, NULL, vars, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK){
        _send_error(err_user_exist, req);
        return _register_clean();
    }
    
    //reply
    evbuffer_add_printf(req->buffer_out, "{username=%s, password=%s}",
                        ue_username, password);
    evhtp_send_reply(req, EVHTP_RES_OK);
    
    _register_clean();
}

//login====================================================
static inline void __login_clean(PGresult *res, char *pw64, char *usertoken)
{
    if (res)
        PQclear(res);
    free(pw64);
    free(usertoken);
}

void cm_login(evhtp_request_t *req, void *arg)
{
    //error
    const int err_param = 1;
    const int err_no_user = 2;
    const int err_wrong_password = 3;
    const int err_db = 4;
    const int err_memc = 5;
    
    PGresult *res = NULL;
    char *pw64 = NULL;
    char *usertoken = NULL;
    
    #define _login_clean() __login_clean(res, pw64, usertoken)
    
    //parse param
    int err = 0;
    const char *username = kvs_find_string(&err, req->uri->query, "username");
    const char *password = kvs_find_string(&err, req->uri->query, "password");
    if (err) {
        _send_error(err_param, req);
        return _login_clean();
    }
    size_t username_len = strlen(username);
    size_t password_len = strlen(password);
    if (username_len == 0 || password_len == 0
            || username_len > UNESC_BUF_MAX || password_len > PASSWORD_MAX) {
        _send_error(err_param, req);
        return _login_clean();
    }
    
    //unescape username
    char buf[UNESC_BUF_MAX+1];
    char *ue_username = buf;
    evhtp_unescape_string((unsigned char**)&ue_username, (unsigned char*)username, username_len+1); //include \0
    username_len = strlen(ue_username);
    if (username_len > USERNAME_MAX) {
        _send_error(err_param, req);
        return _login_clean();
    }
    
    //encrypt password
    char hash[20];
    sha1(hash, password, strlen(password));
    pw64 = base64_cf(hash, 20); //free: free(password_b64);
    
    //sql select
    thread_ctx *pctx = cm_get_thread_ctx();
    PGconn *acdb = pctx->accountdb;
    const char *vars[1] = { 
        ue_username
    };
    res = PQexecParams(acdb,  //free: PQclear(res);
                                 "SELECT id, password FROM user_account WHERE username=$1",
                                 1, NULL, vars, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        _send_error(err_db, req);
        return _login_clean();
    }
    
    int rownums = PQntuples(res);
    if (rownums == 0) {
        _send_error(err_no_user, req);
        return _login_clean();
    }
    
    //userid
    const char* struserid = PQgetvalue(res, 0, 0);
    if (struserid == NULL){
        _send_error(err_db, req);
        return _login_clean();
    }
    
    //password compare
    const char* pw = PQgetvalue(res, 0, 1);
    if (pw == NULL){
        _send_error(err_db, req);
        return _login_clean();
    }
    if (strcmp(pw, pw64) != 0) {
        _send_error(err_wrong_password, req);
        return _login_clean();
    }
    
    //delete old usertoken
    memcached_st *memc = pctx->memc;
    memcached_return_t rc;
    char tokenkey[64];
    snprintf(tokenkey, sizeof(tokenkey), "user_token_%s", ue_username);
    size_t tokenlen = 0;
    char *oldtoken = memcached_get(memc, tokenkey, strlen(tokenkey), &tokenlen, 0, &rc);
    if ( oldtoken ){
        rc = memcached_delete(memc, oldtoken, strlen(oldtoken), 0);
        free(oldtoken);
    }
    
    //generate usertoken
    uuid_t uuid;
    uuid_generate(uuid);
    usertoken = base64_cf((const char*)uuid, sizeof(uuid)); //free: free(usertoken)
    
    //store memcached session
    user_session session;
    session.userid = atoi(struserid);
    
    
    rc = memcached_set(memc, usertoken, strlen(usertoken),
                       (const char*)&session, sizeof(session), SESSION_LIFE_SEC, 0);
    if (rc != MEMCACHED_SUCCESS) {
        _send_error(err_memc, req);
        return _login_clean();
    }
    
    //user token key value set
    rc = memcached_set(memc, tokenkey, strlen(tokenkey),
                       usertoken, strlen(usertoken)+1, SESSION_LIFE_SEC, 0);
    if (rc != MEMCACHED_SUCCESS) {
        _send_error(err_memc, req);
        return _login_clean();
    }
    
    //session
    char cookie[256];
    snprintf(cookie, sizeof(cookie), "usertoken=%s", usertoken);
    evhtp_header_t *header = evhtp_header_new("Set-Cookie", cookie, 0, 1);
    evhtp_headers_add_header(req->headers_out, header);
    
    //reply
    evbuffer_add_printf(req->buffer_out, "{usertoken=%s, tokenlife=%lu, username=%s, password=%s}",
                        usertoken, SESSION_LIFE_SEC, ue_username, password);
    evhtp_send_reply(req, EVHTP_RES_OK);

    _login_clean();
}