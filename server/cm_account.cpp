#include "cm_account.h"
#include "cm_thread.h"
#include "cm_util.h"
#include <postgresql/libpq-fe.h>
#include <uuid/uuid.h>

static const size_t USERNAME_MAX = 40;
static const size_t PASSWORD_MAX = 40;
static const size_t UNESC_BUF_MAX = 100;

//register====================================================
static inline void _send_error(int err, evhtp_request_t* req){
    evbuffer_add_printf(req->buffer_out, "{error=%d}", err);
    evhtp_send_reply(req, EVHTP_RES_BADREQ);
}

void cm_register_account(evhtp_request_t * req, void * arg ) {
    //error
    const int err_param = 1;
    const int err_user_exist = 2;
    
    //parse param
    CmKvs kvs(req->uri->query);
    const char *username = kvs.findString("username");
    const char *password = kvs.findString("password");
    if ( kvs.hasError() ) {
        return _send_error(err_param, req);
    }
    size_t username_len = strlen(username);
    size_t password_len = strlen(password);
    if ( username_len == 0 || password_len == 0
            || username_len > UNESC_BUF_MAX || password_len > PASSWORD_MAX ) {
        return _send_error(err_param, req);
    }
    
    //unescape username
    char buf[UNESC_BUF_MAX+1];
    char *ue_username = buf;
    evhtp_unescape_string((unsigned char**)&ue_username, (unsigned char*)username, username_len+1); //include \0
    username_len = strlen(ue_username);
    if ( username_len > USERNAME_MAX ){
        return _send_error(err_param, req);
    }
    
    //encrypt password
    char hash[20];
    sha1(hash, password, strlen(password));
    char *password_b64 = base64(hash, 20); //free: free(password_b64);
    
    //db
    thread_ctx_t* pctx = cm_get_thread_ctx();
    PGconn *acdb = pctx->accountdb;
    const char *vars[2]={
        ue_username,
        password_b64
    };
    PGresult *res = PQexecParams(acdb,  //free: PQclear(res);
                                 "INSERT INTO user_account (username, password) VALUES($1, $2)",
                                 2, NULL, vars, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK){
        _send_error(err_user_exist, req);
        PQclear(res);
        free(password_b64);
        return;
    }
    
    //reply
    evbuffer_add_printf(req->buffer_out, "{username=%s, password=%s}",
                        ue_username, password);
    evhtp_send_reply(req, EVHTP_RES_OK);
    
    //clean
    PQclear(res);
    free(password_b64);
}

//login====================================================
static inline void _send_error_and_clean(int err, evhtp_request_t *req, PGresult *res, char *password_b64){
    evbuffer_add_printf(req->buffer_out, "{error=%d}", err);
    evhtp_send_reply(req, EVHTP_RES_BADREQ);
    PQclear(res);
    free(password_b64);
}

void cm_login(evhtp_request_t *req, void *arg){
    //error
    const int err_param = 1;
    const int err_no_user = 2;
    const int err_wrong_password = 3;
    const int err_db = 4;
    
    //parse param
    CmKvs kvs(req->uri->query);
    const char *username = kvs.findString("username");
    const char *password = kvs.findString("password");
    if ( kvs.hasError() ) {
        return _send_error(err_param, req);
    }
    size_t username_len = strlen(username);
    size_t password_len = strlen(password);
    if ( username_len == 0 || password_len == 0
            || username_len > UNESC_BUF_MAX || password_len > PASSWORD_MAX ) {
        return _send_error(err_param, req);
    }
    
    //unescape username
    char buf[UNESC_BUF_MAX+1];
    char *ue_username = buf;
    evhtp_unescape_string((unsigned char**)&ue_username, (unsigned char*)username, username_len+1); //include \0
    username_len = strlen(ue_username);
    if ( username_len > USERNAME_MAX ){
        return _send_error(err_param, req);
    }
    
    //encrypt password
    char hash[20];
    sha1(hash, password, strlen(password));
    char *password_b64 = base64(hash, 20); //free: free(password_b64);
    
    //sql select
    thread_ctx_t* pctx = cm_get_thread_ctx();
    PGconn *acdb = pctx->accountdb;
    const char *vars[]={
        ue_username
    };
    PGresult *res = PQexecParams(acdb,  //free: PQclear(res);
                                 "SELECT password FROM user_account WHERE username=$1",
                                 1, NULL, vars, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_TUPLES_OK){
        return _send_error_and_clean(err_db, req, res, password_b64);
    }
    
    //password compare
    const char* pw = PQgetvalue(res, 0, 0);
    if ( pw == NULL ){
        return _send_error_and_clean(err_no_user, req, res, password_b64);
    }
    if ( strcmp(pw, password_b64) != 0 ){
        return _send_error_and_clean(err_wrong_password, req, res, password_b64);
    }
    
    //generate usertoken
    uuid_t uuid;
    uuid_generate(uuid);
    char* usertoken = base64((const char*)uuid, sizeof(uuid)); //free: free(usertoken)
    
    //store memcached session
    
    //reply
    evbuffer_add_printf(req->buffer_out, "{usertoken=%s, username=%s, password=%s}",
                        usertoken, ue_username, password);
    evhtp_send_reply(req, EVHTP_RES_OK);
    
    //clean
    PQclear(res);
    free(password_b64);
    free(usertoken);
}