#ifndef __CM_DIG_H__
#define __CM_DIG_H__

#include <evhtp.h>

//just for test
void cm_img(evhtp_request_t *req, void *arg);

//param-x:int&y:int
void cm_getblock(evhtp_request_t *req, void *arg);

//param-x:int&y:int
void cm_dig(evhtp_request_t *req, void *arg);

//param-use cookie
void cm_diguser_info(evhtp_request_t *req, void *arg);

//param-x:int&y:int
void cm_getcell(evhtp_request_t *req, void *arg);

#endif // __CM_DIG_H__
