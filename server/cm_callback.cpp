#include "cm_callback.h"
#include "cm_util.h"
#include <event2/event.h>
#include <vector>
#include <postgresql/libpq-fe.h>
#include <unistd.h> //for sleep

namespace{
    std::vector<evhtp_callback_t*> cb_vec;
    
    void set_cb(evhtp_t * htp, const char * path, evhtp_callback_cb cb, void * arg){
        evhtp_callback_t *pcb = evhtp_set_cb(htp, path, cb, arg);
        cb_vec.push_back(pcb);
    }
    
    //=================================================
    void register_account(evhtp_request_t * req, void * arg ) {        
//        evhtp_connection_t * htpconn;
//        evthr_t            * thread;

//        htpconn = evhtp_request_get_connection(req);
//        thread  = htpconn->thread;
//        printf("%x\n", (int)thread);
//        sleep(5);
//        unused(NULL);
        
        CmKvs kvs(req->uri->query);
        const char *username = kvs.findString("username");
        const char *password = kvs.findString("password");
        
        if ( kvs.hasError() ){
            evbuffer_add_printf(req->buffer_out, "param error\n");
            evhtp_send_reply(req, EVHTP_RES_BADREQ);
            return;
        }
        evbuffer_add_printf(req->buffer_out, "username=%s, password=%s\n",
                            username, password);
        evhtp_send_reply(req, EVHTP_RES_OK);
    }
}

void cm_register_cbs(evhtp_t *htp){
    set_cb(htp, "/cmapi/register", register_account, NULL);
}

void cm_unregister_cbs(){
    std::vector<evhtp_callback_t*>::iterator it = cb_vec.begin();
    for ( ; it != cb_vec.end(); ++it ){
        evhtp_callback_free(*it);
    }
}