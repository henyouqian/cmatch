#include "cm_rps.h"
#include "cm_account.h"
#include "cm_util.h"
#include "cm_context.h"
#include <list>
#include <hiredis/hiredis.h>

namespace {
    const size_t RPS_LEN = 10;
    
    struct RpsRecord{
        uint64_t userid;
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
    
    //redis
    redisContext *redis = cm_get_context()->redis;
    redisAppendCommand(redis, "MGET last_user_rps_id:%llu curr_rps_id", session.userid);
    char buf[512];
    MemIO mio(buf, sizeof(buf));
    mio.writeUint64(session.userid);
    mio.writeString(session.username);
    mio.writeString(rps);
    redisAppendCommand(redis, "GETSET curr_rps %b", mio.p0, mio.length());
    redisAppendCommand(redis, "INCR curr_rps_id");
    
    //check opponent valid
    //"MGET last_user_rps_id:%llu curr_rps_id"
    redisReply *reply;
    redisGetReply(redis, (void**)&reply);
    bool needrobot = false;
    unsigned int last_user_rps_id = 0;
    unsigned int curr_rps_id = 0;
    if (reply->elements != 2) {
        needrobot = true;
    } else {
        last_user_rps_id = s2uint32(reply->element[0]->str) ;
        curr_rps_id = s2uint32(reply->element[1]->str);
        if ( last_user_rps_id == curr_rps_id ) {
            needrobot = true;
        }
    }
    freeReplyObject(reply);
    
    //redis get opponent rps recored
    //"GETSET curr_rps %b"
    uint64_t oppoid = 0;
    const char *opponame = "";
    const char *opporps = NULL;
    redisGetReply(redis, (void**)&reply);
    if (!needrobot) {
        if (reply->type != REDIS_REPLY_STRING) {
            needrobot = true;
        }
        mio.set(reply->str, reply->len);
        oppoid = mio.readUint64();
        opponame = mio.readString();
        opporps = mio.readString();
    }
    char roborps[RPS_LEN];
    const char *RPS = "rps";
    if (needrobot) {
        oppoid = 0;
        opponame = "robo0";
        //gen rps
        srand(time(NULL));
        
        for (size_t i = 0; i < RPS_LEN; ++i) {
            int n = rand() % 3;
            roborps[i] = RPS[n];
        }
        opporps = roborps;
    }
    evbuffer_add_printf(req->buffer_out, "{\"error\":0, \"oppoid\":%llu, \"opponame\":\"%s\", \"rps\":\"%s\"}", oppoid, opponame, opporps);
    evhtp_send_reply(req, EVHTP_RES_OK);
    freeReplyObject(reply);
    
    //"INCR curr_rps_id"
    redisGetReply(redis, (void**)&reply);
    freeReplyObject(reply);

    reply = (redisReply*)redisCommand(redis, "SET last_user_rps_id:%llu %u", session.userid, curr_rps_id+1);
    freeReplyObject(reply);
}