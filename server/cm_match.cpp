#include "cm_match.h"
#include "cm_util.h"

void cm_dev_add_app(evhtp_request_t *req, void *arg) {
    int err = 0;
    const char *username = kvs_find_string(&err, req->uri->query, "name");
}