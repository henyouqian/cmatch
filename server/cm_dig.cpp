#include "cm_dig.h"
#include "cm_util.h"
#include "cm_account.h"
#include "cm_context.h"
#include <unistd.h>
#include <fcntl.h>
#include <hiredis/hiredis.h>
#include <sstream>

static const int TILE_MAX = 32768;
static const int TILES_PER_BLOCK_SIDE = 16;
static const int BLOCK_MAX = TILE_MAX/TILES_PER_BLOCK_SIDE;

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

//static int getInt(const char **pp, int* err){
//    if (pp == NULL || *pp == NULL) {
//        *err = 1;
//        return 0;
//    }
//    if (err == NULL) {
//        return 0;
//    }
//    if (*err) {
//        return 0;
//    }
//    
//    const char* p0 = *pp;
//    while ((**pp) != ',' && (**pp) != 0) {
//        ++(*pp);
//    }
//    char buf[32] = {0};
//    if ( *pp != p0 )
//        memcpy(buf, p0, (*pp)-p0);
//        
//    int n = s2int32(buf, err);
//    if ((**pp) != 0)
//        ++(*pp);
//    return n;
//}

void cm_getblock(evhtp_request_t *req, void *arg) {
    enum {
        err_param = -1,
        err_db = -2,
    };
    
    evbuffer *evbuf = req->buffer_in;
    int sz = evbuffer_get_length(evbuf);
    char *inbuf = (char*)malloc(sz + 1);
    Autofree _af_buf(inbuf);
    evbuffer_copyout(evbuf, inbuf, sz);
    inbuf[sz] = 0;
    
    std::stringstream ss;
    ss << "MGET";
    CommaReader commaReader(inbuf);
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
    
//    const char* p = (const char*)inbuf;
//    std::vector<int> xys;
//    while (1) {
//        int x = getInt(&p, &err);
//        int y = getInt(&p, &err);
//        if (x < 0 || x >= BLOCK_MAX || y < 0 || y >= BLOCK_MAX) {
//            err = err_param;
//        }
//        if (err)
//            break;
//        xys.push_back(x);
//        xys.push_back(y);
//    }
//    if (xys.empty()) {
//        return cm_send_error(err_param, req);
//    }
//    
//    std::stringstream ss;
//    ss << "MGET";
//    std::vector<int>::iterator it = xys.begin();
//    std::vector<int>::iterator end = xys.end();
//    for (; it != end; ) {
//        int x = *it;
//        ++it;
//        int y = *it;
//        ++it;
//        ss << " block:" << x << "," << y; 
//    }
    
    redisContext *redis = cm_get_context()->redis;
    redisReply *reply = (redisReply*)redisCommand(redis, ss.str().c_str());
    if (reply == NULL){
        return cm_send_error(err_param, req);
    }
        
    std::stringstream out;
    out << "{\"error\":0, \"data\":{";
    char buf[TILES_PER_BLOCK_SIDE*TILES_PER_BLOCK_SIDE/8] = {0};
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
    if (x < 0 || x >= TILE_MAX || y < 0 || y >= TILE_MAX) {
        return cm_send_error(err_param, req);
    }
    int undig = kvs_find_int(&err, req->uri->query, "undig");
    
    int blockX = x/TILES_PER_BLOCK_SIDE;
    int blockY = y/TILES_PER_BLOCK_SIDE;
    int idx = (y%TILES_PER_BLOCK_SIDE)*TILES_PER_BLOCK_SIDE+(x%TILES_PER_BLOCK_SIDE);
    
    redisContext *redis = cm_get_context()->redis;
    if (undig)
        redisAppendCommand(redis, "SETBIT block:%u,%u %u 0", blockX, blockY, idx);
    else
        redisAppendCommand(redis, "SETBIT block:%u,%u %u 1", blockX, blockY, idx);
    redisAppendCommand(redis, "GET block:%u,%u", blockX, blockY);
    
    redisReply *reply;
    redisGetReply(redis, (void**)&reply);
    freeReplyObject(reply);
    
    redisGetReply(redis, (void**)&reply);
    char buf[TILES_PER_BLOCK_SIDE*TILES_PER_BLOCK_SIDE/8] = {0};
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