#include "cm_account.h"
#include "cm_util.h"
#include "cm_context.h"
#include <postgresql/libpq-fe.h>
#include <uuid/uuid.h>
#include <map>
#include <list>
#include <string>

static const size_t UNESC_BUF_MAX = 100;
static const time_t SESSION_LIFE_SEC = 60*60;   //one hour

static void newSession(std::string &newtoken, cm_session& insession) {
    uuid_t uuid;
    uuid_generate(uuid);
    char *token = base64_cf((const char*)uuid, sizeof(uuid));
    Autofree af_token(token);
    newtoken = token;
    
    redisContext *redis = cm_get_context()->redis;
    redisReply *reply = (redisReply*)redisCommand(redis, "SETEX session:%s %u %b",
                                     token, SESSION_LIFE_SEC, &insession, sizeof(insession));
    freeReplyObject(reply);
}

static void delSession(const char* token) {
    redisContext *redis = cm_get_context()->redis;
    redisReply *reply = (redisReply*)redisCommand(redis, "DEL session:%s", token);
    freeReplyObject(reply);
}

static int findSession(const char *token, cm_session& session) {
    redisContext *redis = cm_get_context()->redis;
    redisReply *reply = (redisReply*)redisCommand(redis, "GET session:%s", token);
    int r = 0;
    if (reply->type == REDIS_REPLY_STRING && reply->len == sizeof(session)) {
        memcpy(&session, reply->str, sizeof(session));
    } else {
        r = -1;
    }
    freeReplyObject(reply);
    return r;
}

//====================================================
int cm_find_session(evhtp_request_t *req, cm_session &session) {
    enum{
        err_no_cookie = -1,
        err_not_found = -2,
    };
    char *usertoken = findCookie_cf(req, "usertoken");
    if (!usertoken) {
        return err_no_cookie;
    }
    Autofree af_usertoken(usertoken);
    
    int err = findSession(usertoken, session);
    if (err) {
        return err_not_found;
    }
    return 0;
}

void cm_send_error(int err, evhtp_request_t* req) {
    evbuffer_add_printf(req->buffer_out, "{\"error\":%d}", err);
    evhtp_send_reply(req, EVHTP_RES_OK);
}

void cm_send_ok(evhtp_request_t* req) {
    evbuffer_add_printf(req->buffer_out, "{\"error\":0}");
    evhtp_send_reply(req, EVHTP_RES_OK);
}

//register====================================================
void cm_register(evhtp_request_t *req, void *arg) {
    //error def
    enum{
        err_param = -1,
        err_user_exist = -2,
    };
    
    //parse param
    int err = 0;
    const char *username = kvs_find_string(&err, req->uri->query, "username");
    const char *password = kvs_find_string(&err, req->uri->query, "password");
    if (err) {
        cm_send_error(err_param, req);
        return;
    }
    size_t username_len = strlen(username);
    size_t password_len = strlen(password);
    if (username_len == 0 || password_len == 0
            || username_len > UNESC_BUF_MAX || password_len > PASSWORD_MAX) {
        cm_send_error(err_param, req);
        return;
    }
    
    //unescape username
    char buf[UNESC_BUF_MAX+1] = {0};
    char *ue_username = buf;
    evhtp_unescape_string((unsigned char**)&ue_username, (unsigned char*)username, username_len+1); //include \0
    username_len = strlen(ue_username);
    if (username_len > USERNAME_MAX) {
        cm_send_error(err_param, req);
        return;
    }
    
    //encrypt password
    char hash[20];
    sha1(hash, password, strlen(password));
    char *pw64 = base64_cf(hash, 20);
    Autofree af_pw64(pw64);
    
    //db
    cm_context* pctx = cm_get_context();
    PGconn *acdb = pctx->accountdb;
    const char *vars[2] = {
        ue_username,
        pw64,
    };

    PGresult *res = PQexecParams(acdb,
                                 "INSERT INTO user_account (username, password) VALUES($1, $2)",
                                 2, NULL, vars, NULL, NULL, 0);
    Autofree af_res(res, (Freefunc)PQclear);
    if (PQresultStatus(res) != PGRES_COMMAND_OK){
        cm_send_error(err_user_exist, req);
        return;
    }
    
    //reply
    evbuffer_add_printf(req->buffer_out, "{\"error\":0, \"username\":\"%s\", \"password\":\"%s\"}",
                        ue_username, password);
    evhtp_send_reply(req, EVHTP_RES_OK);
}

//login====================================================
void cm_login(evhtp_request_t *req, void *arg) {
    //error
    enum{
        err_param = -1,
        err_no_user = -2,
        err_wrong_password = -3,
        err_db = -4,
        err_memc = -5,
    };
    
    //parse param
    int err = 0;
    const char *username = kvs_find_string(&err, req->uri->query, "username");
    const char *password = kvs_find_string(&err, req->uri->query, "password");
    if (err) {
        cm_send_error(err_param, req);
        return;
    }
    size_t username_len = strlen(username);
    size_t password_len = strlen(password);
    if (username_len == 0 || password_len == 0
            || username_len > UNESC_BUF_MAX || password_len > PASSWORD_MAX) {
        cm_send_error(err_param, req);
        return;
    }
    
    //unescape username
    char buf[UNESC_BUF_MAX+1];
    char *ue_username = buf;
    evhtp_unescape_string((unsigned char**)&ue_username, (unsigned char*)username, username_len+1); //include \0
    username_len = strlen(ue_username);
    if (username_len > USERNAME_MAX) {
        cm_send_error(err_param, req);
        return;
    }
    
    //encrypt password
    char hash[20];
    sha1(hash, password, strlen(password));
    char *pw64 = base64_cf(hash, 20);
    Autofree af_pw64(pw64);
    
    //sql select
    cm_context* pctx = cm_get_context();
    PGconn *acdb = pctx->accountdb;
    const char *vars[1] = { 
        ue_username
    };
    PGresult *res = PQexecParams(acdb,  //free: PQclear(res);
                                 "SELECT id, password FROM user_account WHERE username=$1",
                                 1, NULL, vars, NULL, NULL, 0);
    Autofree af_res(res, (Freefunc)PQclear);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        cm_send_error(err_db, req);
        return;
    }
    
    int rownums = PQntuples(res);
    if (rownums == 0) {
        cm_send_error(err_no_user, req);
        return;
    }
    
    //userid
    const char* struserid = PQgetvalue(res, 0, 0);
    if (struserid == NULL){
        cm_send_error(err_db, req);
        return;
    }
    
    //password compare
    const char* pw = PQgetvalue(res, 0, 1);
    if (pw == NULL){
        cm_send_error(err_db, req);
        return;
    }
    if (strcmp(pw, pw64) != 0) {
        cm_send_error(err_wrong_password, req);
        return;
    }
    
    //session
    char *oldtoken = findCookie_cf(req, "usertoken");
    cm_session session;
    session.userid = atoi(struserid);
    strncpy(session.username, ue_username, sizeof(session.username));
    std::string usertoken;
    if (oldtoken) {
        delSession(oldtoken);
    }
    newSession(usertoken, session);
    
    //cookie
    char cookie[256];
    snprintf(cookie, sizeof(cookie), "usertoken=%s; path=/", usertoken.c_str());
    evhtp_header_t *header = evhtp_header_new("Set-Cookie", cookie, 0, 1);
    evhtp_headers_add_header(req->headers_out, header);
    snprintf(cookie, sizeof(cookie), "username=%s; path=/", ue_username);
    header = evhtp_header_new("Set-Cookie", cookie, 0, 1);
    evhtp_headers_add_header(req->headers_out, header);
    
    //reply
    evbuffer_add_printf(req->buffer_out, "{\"error\":0}");
    evhtp_send_reply(req, EVHTP_RES_OK);
}

void cm_relogin(evhtp_request_t *req, void *arg) {
    //error def
    enum{
        err_nologin = -1,
    };
    
    //find and renew
    cm_session session;
    int err = cm_find_session(req, session);
    if (err) {
        cm_send_error(err_nologin, req);
        return;
    }
    char *usertoken = findCookie_cf(req, "usertoken");
    Autofree af_usertoken(usertoken);
    delSession(usertoken);
    std::string strusertoken;
    newSession(strusertoken, session);
    
    //cookie
    char cookie[256];
    snprintf(cookie, sizeof(cookie), "usertoken=%s; path=/", strusertoken.c_str());
    evhtp_header_t *header = evhtp_header_new("Set-Cookie", cookie, 0, 1);
    evhtp_headers_add_header(req->headers_out, header);
    
    evbuffer_add_printf(req->buffer_out, "{\"error\":0}");
    evhtp_send_reply(req, EVHTP_RES_OK);
}

void cm_logout(evhtp_request_t *req, void *arg) {
    //error def
    enum{
        err_param = -1,
        err_nologin = -2,
    };
    
    //parse cookie
    char *usertoken = findCookie_cf(req, "usertoken");
    Autofree af_usertoken(usertoken);
    if (!usertoken) {
        cm_send_error(err_param, req);
        return;
    }
    
    //delete usertoken
    delSession(usertoken);
    
    evbuffer_add_printf(req->buffer_out, "{\"error\":0}");
    evhtp_send_reply(req, EVHTP_RES_OK);
}