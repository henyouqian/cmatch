#ifndef __CM_ACCOUNT_H__
#define __CM_ACCOUNT_H__

#include <evhtp.h>
#include <stdint.h>

struct user_session {
    uint64_t userid;
};

//username=&password=
void cm_register_account(evhtp_request_t *req, void *arg);
//username=&password=
void cm_login(evhtp_request_t *req, void *arg);
//no param, use cookie
void cm_relogin(evhtp_request_t *req, void *arg);

#endif // __CM_ACCOUNT_H__
