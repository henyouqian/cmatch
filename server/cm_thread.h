#ifndef __CM_THREAD_H__
#define __CM_THREAD_H__

#include <evhtp.h>
#include <postgresql/libpq-fe.h>
#include <libmemcached/memcached.h>

extern const int g_thread_nums;

struct thread_ctx{
    PGconn *accountdb;
    PGconn *cmatchdb;
    memcached_st *memc;
};
thread_ctx *cm_get_thread_ctx();

void cm_thread_init_cb(evhtp_t *htp, evthr_t *thr, void *arg);

#endif // __CM_THREAD_H__
