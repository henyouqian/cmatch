#ifndef __CM_MATCH_H__
#define __CM_MATCH_H__

#include <evhtp.h>

//@param
//use cookie
void cmdev_list_apps(evhtp_request_t *req, void *arg);

//@param
//appname:string
void cmdev_add_app(evhtp_request_t *req, void *arg);

//@param
//appid:id & appname:string
void cmdev_edit_app(evhtp_request_t *req, void *arg);

//@param
//appid:id
void cmdev_get_app_secret(evhtp_request_t *req, void *arg);

//@param
void cmdev_add_game(evhtp_request_t *req, void *arg);

//@param
//
void cmdev_add_match(evhtp_request_t *req, void *arg);


//@param
//matchid:id & userid:id
//@return
//matchtoken:string & expire:int
void cm_request_play(evhtp_request_t *req, void *arg);

//@param
//matchtoken:string & play:binary
void cm_submit_play(evhtp_request_t *req, void *arg);

#endif // __CM_MATCH_H__
