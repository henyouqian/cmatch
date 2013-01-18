#ifndef __CM_CONTEXT_H__
#define __CM_CONTEXT_H__

#include <postgresql/libpq-fe.h>
#include <hiredis/hiredis.h>
#include <pthread.h>

struct cm_context {
    PGconn *accountdb;
    PGconn *cmatchdb;
    redisContext *redis;
    pthread_t tid;
};

cm_context *cm_get_context();

#endif // __CM_CONTEXT_H__
