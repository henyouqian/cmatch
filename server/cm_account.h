#ifndef __CM_ACCOUNT_H__
#define __CM_ACCOUNT_H__

#include <evhtp.h>
#include <stdint.h>

enum {
    USERNAME_MAX = 40,
    PASSWORD_MAX = 40,
};

struct cm_session {
    uint64_t userid;
    char username[USERNAME_MAX];
};

void cm_send_error(int err, evhtp_request_t* req);
void cm_send_ok(evhtp_request_t* req);

//return err_no_cookie = -1, err_expired = -2
int cm_find_session(evhtp_request_t *req, cm_session &session);

//username=string&password=string
//return err_param = -1, err_user_exist = -2,
void cm_register(evhtp_request_t *req, void *arg);

//username=string&password=string
//return err_param = -1, err_no_user = -2, err_wrong_password = -3, err_db = -4, err_memc = -5
void cm_login(evhtp_request_t *req, void *arg);

//cookie param: usertoken=string
//return err_param = -1, err_nologin = -2
void cm_relogin(evhtp_request_t *req, void *arg);

//cookie param: usertoken=string
//return err_param = -1, err_nologin = -2
void cm_logout(evhtp_request_t *req, void *arg);

//register or login
//username:string&password:string
void cm_reglog(evhtp_request_t *req, void *arg);

#endif // __CM_ACCOUNT_H__
