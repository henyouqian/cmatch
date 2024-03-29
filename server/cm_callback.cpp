#include "cm_callback.h"
#include "cm_account.h"
#include "cm_rps.h"
#include "cm_dig.h"
#include "cm_match.h"
#include <vector>

namespace {
    std::vector<evhtp_callback_t*> cb_vec;
    
    void set_cb(evhtp_t * htp, const char * path, evhtp_callback_cb cb, void * arg) {
        evhtp_callback_t *pcb = evhtp_set_cb(htp, path, cb, arg);
        cb_vec.push_back(pcb);
    }
}

void cm_register_cbs(evhtp_t *htp) {
    set_cb(htp, "/cmapi/register", cm_register, NULL);
    set_cb(htp, "/cmapi/login", cm_login, NULL);
    set_cb(htp, "/cmapi/relogin", cm_relogin, NULL);
    set_cb(htp, "/cmapi/logout", cm_logout, NULL);
    set_cb(htp, "/cmapi/reglog", cm_reglog, NULL);
    
    set_cb(htp, "/cmapi/rps", cm_rps, NULL);
    set_cb(htp, "/cmapi/img", cm_img, NULL);
    set_cb(htp, "/cmapi/getblock", cm_getblock, NULL);
    set_cb(htp, "/cmapi/dig", cm_dig, NULL);
    set_cb(htp, "/cmapi/diguserinfo", cm_diguser_info, NULL);
    set_cb(htp, "/cmapi/getcell", cm_getcell, NULL);
    
    set_cb(htp, "/cmapi/dev/listapps", cmdev_list_apps, NULL);
    set_cb(htp, "/cmapi/dev/addapp", cmdev_add_app, NULL);
    set_cb(htp, "/cmapi/dev/editapp", cmdev_edit_app, NULL);
    set_cb(htp, "/cmapi/dev/getappsecrit", cmdev_get_app_secret, NULL);
    
}

void cm_free_cbs() {
    std::vector<evhtp_callback_t*>::iterator it = cb_vec.begin();
    for (; it != cb_vec.end(); ++it) {
        evhtp_callback_free(*it);
    }
}