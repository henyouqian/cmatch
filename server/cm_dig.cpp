#include "cm_dig.h"
#include "cm_util.h"
#include "cm_account.h"
#include "cm_context.h"
#include <unistd.h>
#include <fcntl.h>
#include <hiredis/hiredis.h>
#include <sstream>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

static const int CELL_MAX = 32768;
static const int CELLS_PER_BLOCK_SIDE = 16;
static const int BLOCK_MAX = CELL_MAX/CELLS_PER_BLOCK_SIDE;

void cm_img(evhtp_request_t *req, void *arg) {
    int fd = open("../resource/players.png", O_RDONLY);
    if (fd == -1){
        evhtp_send_reply(req, EVHTP_RES_OK);
        return;
    }
    int len = lseek(fd , 0 , SEEK_END);
    evbuffer_add_file(req->buffer_out, fd, 0, len);
    evhtp_send_reply(req, EVHTP_RES_OK);
    //close(fd);
}


void cm_getblock(evhtp_request_t *req, void *arg) {
    enum {
        err_param = -1,
        err_db = -2,
        err_etc = -3,
        err_needlogin = -4,
    };
    
    evbuffer *evbuf = req->buffer_in;
    int sz = evbuffer_get_length(evbuf);
    char *inbuf = (char*)malloc(sz + 1);
    Autofree _af_buf(inbuf);
    evbuffer_copyout(evbuf, inbuf, sz);
    inbuf[sz] = 0;
    
    CommaReader commaReader(inbuf);
    float posX, posY;
    commaReader.readFloat(posX);
    commaReader.readFloat(posY);
    if (commaReader.getStatus()){
        return cm_send_error(err_param, req);
    }
    
    redisContext *redis = cm_get_context()->redis;
    if (redis->err) {
        return cm_send_error(err_db, req);
    }
    
    cm_session session;
    int err = cm_find_session(req, session);
    if (err == 0) {
        char buf[128];
        int n = snprintf(buf, sizeof(buf), "HMSET diguser:%" PRIu64 " x %.1f y %.1f", session.userid, posX, posY);
        if (n < 0) {
            return cm_send_error(err_etc, req);
        }
        redisReply *reply = (redisReply*)redisCommand(redis, buf);
        if (reply == NULL){
            return cm_send_error(err_db, req);
        }
        freeReplyObject(reply);
    } if (err == -2) { //err_expired
        return cm_send_error(err_needlogin, req);
    }
    
    std::stringstream ss;
    ss << "MGET";
    size_t nBlock = 0;
    std::vector<int> xys;
    while (1) {
        int x, y;
        commaReader.readInt(x);
        commaReader.readInt(y);
        int status = commaReader.getStatus();
        if (status < 0)
            break;
        if (x < 0 || x >= BLOCK_MAX || y < 0 || y >= BLOCK_MAX)
            break;
            
        xys.push_back(x);
        xys.push_back(y);
        ss << " block:" << x << "," << y;
        ++nBlock;
        if (status > 0)
            break;
    }
    if (nBlock==0) {
        return cm_send_error(err_param, req);
    }
    
    redisReply *reply = (redisReply*)redisCommand(redis, ss.str().c_str());
    if (reply == NULL){
        return cm_send_error(err_param, req);
    }
        
    std::stringstream out;
    out << "{\"error\":0, \"data\":{";
    char buf[CELLS_PER_BLOCK_SIDE*CELLS_PER_BLOCK_SIDE/8] = {0};
    if (reply->type == REDIS_REPLY_ARRAY && reply->elements == nBlock) {
        for (size_t i = 0; i < reply->elements; ++i) {
            redisReply *rp = reply->element[i];
            if (rp->str && rp->len <= (int)sizeof(buf)) {
                memset(buf, 0, sizeof(buf));
                memcpy(buf, rp->str, rp->len);
                char *b64 = base64_cf(buf, sizeof(buf));
                if (i > 0)
                    out << ",";
                out << "\"" << xys[i*2] << "," << xys[i*2+1] << "\":\"" << b64 << "\"";
                free(b64);
            } else {
                if (i > 0)
                    out << ",";
                out << "\"" << xys[i*2] << "," << xys[i*2+1] << "\":" << 0;
            }
        }
    } else {
        cm_send_error(err_db, req);
    }
    out << "}}";
    evbuffer_add_printf(req->buffer_out, "%s", out.str().c_str());
    evhtp_send_reply(req, EVHTP_RES_OK);
    freeReplyObject(reply);
}

void cm_dig(evhtp_request_t *req, void *arg) {
    enum {
        err_param = -1,
        err_db = -2,
    };
    int err = 0;
    int x = kvs_find_int(&err, req->uri->query, "x");
    int y = kvs_find_int(&err, req->uri->query, "y");
    if (err) {
        return cm_send_error(err_param, req);
    }
    if (x < 0 || x >= CELL_MAX || y < 0 || y >= CELL_MAX) {
        return cm_send_error(err_param, req);
    }
    int undig = kvs_find_int(&err, req->uri->query, "undig");
    
    int blockX = x/CELLS_PER_BLOCK_SIDE;
    int blockY = y/CELLS_PER_BLOCK_SIDE;
    int idx = (y%CELLS_PER_BLOCK_SIDE)*CELLS_PER_BLOCK_SIDE+(x%CELLS_PER_BLOCK_SIDE);
    
    redisContext *redis = cm_get_context()->redis;
    if (redis->err) {
        return cm_send_error(err_db, req);
    }
    if (undig)
        redisAppendCommand(redis, "SETBIT block:%u,%u %u 0", blockX, blockY, idx);
    else
        redisAppendCommand(redis, "SETBIT block:%u,%u %u 1", blockX, blockY, idx);
    redisAppendCommand(redis, "GET block:%u,%u", blockX, blockY);
    
    redisReply *reply;
    redisGetReply(redis, (void**)&reply);
    freeReplyObject(reply);
    
    redisGetReply(redis, (void**)&reply);
    char buf[CELLS_PER_BLOCK_SIDE*CELLS_PER_BLOCK_SIDE/8] = {0};
    if (reply->type == REDIS_REPLY_STRING && reply->len <= (int)sizeof(buf)) {
        memcpy(buf, reply->str, reply->len);
        char *b64 = base64_cf(buf, sizeof(buf));
        evbuffer_add_printf(req->buffer_out, "{\"error\":0, \"data\":\"%s\"}", b64);
        evhtp_send_reply(req, EVHTP_RES_OK);
        free(b64);
    } else {
        cm_send_error(err_db, req);
    }
    
    freeReplyObject(reply);
    
}

void cm_diguser_info(evhtp_request_t *req, void *arg) {
    enum {
        err_nologin = -1,
        err_db = -2,
    };
    redisContext *redis = cm_get_context()->redis;
    if (redis->err) {
        return cm_send_error(err_db, req);
    }
    
    cm_session session;
    int err = cm_find_session(req, session);
    if (err) {
        return cm_send_error(err_nologin, req);
    }
    redisReply *reply = (redisReply*)redisCommand(redis, "HMGET diguser:%" PRIu64 " x y",session.userid);
    Autofree _af_reply(reply, freeReplyObject);
    if (reply == NULL || reply->type != REDIS_REPLY_ARRAY || reply->elements != 2) {
        return cm_send_error(err_db, req);
    }
    const char *x = reply->element[0]->str;
    const char *y = reply->element[1]->str;
    if (x == NULL || y == NULL) { //not saved
        x = "-1";
        y = "-1";
    }
    evbuffer_add_printf(req->buffer_out, "{\"error\":0, \"x\":\"%s\", \"y\":\"%s\"}", x, y);
    evhtp_send_reply(req, EVHTP_RES_OK);
}

void cm_getcell(evhtp_request_t *req, void *arg) {
    enum {
        err_param = -1,
        err_db = -2,
    };
    int err = 0;
    int x = kvs_find_int(&err, req->uri->query, "x");
    int y = kvs_find_int(&err, req->uri->query, "y");
    if (err) {
        return cm_send_error(err_param, req);
    }
    if (x < 0 || x >= CELL_MAX || y < 0 || y >= CELL_MAX) {
        return cm_send_error(err_param, req);
    }
    
    redisContext *redis = cm_get_context()->redis;
    if (redis->err) {
        return cm_send_error(err_db, req);
    }
//    int blockX = x / CELLS_PER_BLOCK_SIDE;
//    int blockY = y / CELLS_PER_BLOCK_SIDE;
//    int idx = (y%CELLS_PER_BLOCK_SIDE)*CELLS_PER_BLOCK_SIDE+(x%CELLS_PER_BLOCK_SIDE);
    redisReply *reply = (redisReply*)redisCommand(redis, "HMGET cell:%d,%d text hp",x, y);
    Autofree _af_reply(reply, freeReplyObject);
    if (reply == NULL || reply->type != REDIS_REPLY_ARRAY || reply->elements != 2) {
        return cm_send_error(err_db, req);
    }
    const char *text = reply->element[0]->str;
    const char *hp = reply->element[1]->str;
    if (text == NULL || hp == NULL) {
        text = "";
        hp = "0";
    }
    evbuffer_add_printf(req->buffer_out, "{\"error\":0, \"text\":\"%s\", \"hp\":\"%s\"}", text, hp);
    evhtp_send_reply(req, EVHTP_RES_OK);
}