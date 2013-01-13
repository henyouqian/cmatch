#include "cm_dig.h"
#include "cm_util.h"
#include "cm_account.h"
#include "cm_context.h"
#include <unistd.h>
#include <fcntl.h>
#include <hiredis/hiredis.h>


static const int TILE_MAX = 32768;

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
    };
    int err = 0;
    int x = kvs_find_int(&err, req->uri->query, "x");
    int y = kvs_find_int(&err, req->uri->query, "y");
    if (err) {
        cm_send_error(err_param, req);
        return;
    }
    if (x < 0 || x >= TILE_MAX || y < 0 || y >= TILE_MAX) {
        cm_send_error(err_param, req);
        return;
    }
    
    redisContext *redis = cm_get_context()->redis;
    redisReply *reply = (redisReply*)redisCommand(redis, "GET block:%u,%u", x, y);
    char buf[32] = {0};
    if (reply->type == REDIS_REPLY_STRING && reply->len == sizeof(buf)) {
        memcpy(buf, reply->str, sizeof(buf));
    }
    char *b64 = base64_cf(buf, sizeof(buf));
    evbuffer_add_printf(req->buffer_out, "%s", b64);
    evhtp_send_reply(req, EVHTP_RES_OK);
    
    freeReplyObject(reply);
    free(b64);
}

void cm_dig(evhtp_request_t *req, void *arg) {
    
}