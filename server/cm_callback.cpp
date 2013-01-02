#include "cm_callback.h"
#include "cm_account.h"
#include <vector>

namespace
{
    std::vector<evhtp_callback_t*> cb_vec;
    
    void set_cb(evhtp_t * htp, const char * path, evhtp_callback_cb cb, void * arg){
        evhtp_callback_t *pcb = evhtp_set_cb(htp, path, cb, arg);
        cb_vec.push_back(pcb);
    }
}

void cm_register_cbs(evhtp_t *htp)
{
    set_cb(htp, "/cmapi/register", cm_register_account, NULL);
    set_cb(htp, "/cmapi/login", cm_login, NULL);
    set_cb(htp, "/cmapi/relogin", cm_relogin, NULL);
}

void cm_free_cbs()
{
    std::vector<evhtp_callback_t*>::iterator it = cb_vec.begin();
    for (; it != cb_vec.end(); ++it) {
        evhtp_callback_free(*it);
    }
}