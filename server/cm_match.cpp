#include "cm_match.h"
#include <event2/event.h>
#include <vector>

namespace{
    std::vector<evhtp_callback_t*> cb_vec;
    
    void set_cb(evhtp_t * htp, const char * path, evhtp_callback_cb cb, void * arg){
        evhtp_callback_t *pcb = evhtp_set_cb(htp, path, cb, arg);
        cb_vec.push_back(pcb);
    }
    
    //=================================================
    void register_account(evhtp_request_t * req, void * arg ) {
        evbuffer_add_printf(req->buffer_out, "register_account\n");
        const char *p = evhtp_kv_find(req->uri->query, "a");
        if ( p ){
            printf("%s\n",p);
        }
        //evbuffer_add_reference(req->buffer_out, "register_account\n", 12, NULL, NULL);
        evhtp_send_reply(req, EVHTP_RES_OK);
    }
}

void register_match_cb(evhtp_t *htp){
    set_cb(htp, "/register", register_account, NULL);
    
}

void free_match_cb(){
    std::vector<evhtp_callback_t*>::iterator it = cb_vec.begin();
    for ( ; it != cb_vec.end(); ++it ){
        evhtp_callback_free(*it);
    }
}