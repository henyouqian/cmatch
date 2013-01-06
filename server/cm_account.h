#ifndef __CM_ACCOUNT_H__
#define __CM_ACCOUNT_H__

#include <evhtp.h>
#include <stdint.h>

struct cm_session {
    uint64_t userid;
    time_t expire;
    bool _deleted;
};

enum {
    cmerr_param = 1,
    cmerr_nologin = 2,
};

void cm_send_error(int err, evhtp_request_t* req);
void cm_send_ok(evhtp_request_t* req);

//return err_no_cookie = -1, err_not_found = -2
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

#endif // __CM_ACCOUNT_H__
