#include "cm_dig.h"
#include "cm_util.h"
#include <unistd.h>
#include <fcntl.h>

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
    int rgba = 0x1020304;
    char* b64 = base64_cf((void*)&rgba, sizeof(rgba));
    evbuffer_add_printf(req->buffer_out, "{\"data\":\"%s\"}", b64);
    evhtp_send_reply(req, EVHTP_RES_OK);
    free(b64);
}