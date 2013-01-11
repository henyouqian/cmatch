#ifndef __CM_CONTEXT_H__
#define __CM_CONTEXT_H__

#include <postgresql/libpq-fe.h>
#include <hiredis/hiredis.h>

struct cm_context {
    PGconn *accountdb;
    PGconn *cmatchdb;
    redisContext *redis;
};

cm_context *cm_get_context();

#endif // __CM_CONTEXT_H__
