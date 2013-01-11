#include "cm_context.h"
#include <assert.h>

static cm_context _cm_context;

namespace {
    class ContextLife {
    public:
        ContextLife() {
            //pq connection
            _cm_context.accountdb = PQsetdbLogin("127.0.0.1","5432","","","account_db","postgres","nmmgbnmmgb");
            if (PQstatus(_cm_context.accountdb) != CONNECTION_OK) {
                fprintf(stderr, "Connection to account_db failed: %s",
                        PQerrorMessage(_cm_context.accountdb));
                assert(0);
            }
            _cm_context.cmatchdb = PQsetdbLogin("127.0.0.1","5432","","","cmatch_db","postgres","nmmgbnmmgb");
            if (PQstatus(_cm_context.cmatchdb) != CONNECTION_OK) {
                fprintf(stderr, "Connection to cmatch_db failed: %s",
                        PQerrorMessage(_cm_context.cmatchdb));
                assert(0);
            }
            
            //redis
            _cm_context.redis = redisConnect("127.0.0.1", 6379);
        }
        ~ContextLife() {
            PQfinish(_cm_context.accountdb);
            PQfinish(_cm_context.cmatchdb);
            redisFree(_cm_context.redis);
        }
    };
    
    ContextLife _contextlife;
}

cm_context *cm_get_context() {
    return &_cm_context;
}