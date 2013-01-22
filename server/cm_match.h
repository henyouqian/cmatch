#ifndef __CM_MATCH_H__
#define __CM_MATCH_H__

#include <evhtp.h>

//@param
//name=tring
void cm_dev_add_app(evhtp_request_t *req, void *arg);

//@param
void cm_dev_add_game(evhtp_request_t *req, void *arg);

//@param
//
void cm_dev_add_match(evhtp_request_t *req, void *arg);


//@param
//matchid:id & userid:id
//@return
//matchtoken:string & expire:int
void cm_request_play(evhtp_request_t *req, void *arg);

//@param
//matchtoken:string & play:binary
void cm_submit_play(evhtp_request_t *req, void *arg);

#endif // __CM_MATCH_H__
