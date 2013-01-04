#include "cm_rps.h"
#include "cm_account.h"
#include "cm_util.h"

namespace {
    const size_t RPS_LEN = 10;
}

void cm_rps(evhtp_request_t *req, void *arg) {
    enum{
        err_nologin = -1,
        err_param = -2,
    };
    
    //check auth
    cm_session session;
    int err = cm_find_session(req, session);
    if (err) {
        cm_send_error(err_nologin, req);
        return;
    }
    
    //parse rps
    const char *rps = kvs_find_string(&err, req->uri->query, "rps");
    if (strlen(rps) != RPS_LEN) {
        cm_send_error(err_param, req);
        return;
    }
    
}