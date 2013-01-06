#include "cm_rps.h"
#include "cm_account.h"
#include "cm_util.h"
#include "cm_thread.h"
#include <list>
#include <hiredis/hiredis.h>

namespace {
    const size_t RPS_LEN = 10;
    const char *rps_list = "rps_list";
    
    struct RpsElem{
        char data[RPS_LEN];
    };

}

void cm_rps(evhtp_request_t *req, void *arg) {
    //check auth
    cm_session session;
    int err = cm_find_session(req, session);
    if (err) {
        cm_send_error(cmerr_nologin, req);
        return;
    }
    
    //parse rps
    const char *rps = kvs_find_string(&err, req->uri->query, "data");
    if (rps == NULL || strlen(rps) != RPS_LEN) {
        cm_send_error(cmerr_param, req);
        return;
    }
    
    //rps user session
    
    
    //find opponent
    redisContext *redis = cm_get_thread_ctx()->redis;
    //bool found = false;
    redisReply *reply = (redisReply*)redisCommand(redis, "LINDEX %s 0", rps_list);
    
    if (reply->type == REDIS_REPLY_NIL )
    freeReplyObject(reply);
    
    cm_send_ok(req);
}