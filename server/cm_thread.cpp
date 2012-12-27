#include "cm_thread.h"
#include <assert.h>

const int g_thread_nums = 8;
static pthread_key_t thrkey;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;

static void free_func(void* arg){
    thread_ctx_t* pctx = (thread_ctx_t*)arg;
    PQfinish(pctx->accountdb);
    PQfinish(pctx->cmatchdb);
}

static void init_once(void){
    pthread_key_create(&thrkey, free_func);
}

thread_ctx_t* cm_get_thread_ctx(){
    return (thread_ctx_t*)(pthread_getspecific(thrkey));
}

void cm_thread_init_cb(evhtp_t * htp, evthr_t * thr, void * arg){
    pthread_once(&init_done, init_once);
    
    thread_ctx_t *pctx = (thread_ctx_t*)malloc(sizeof(thread_ctx_t));
    pthread_setspecific(thrkey, pctx);
    
    //pq connection
    pctx->accountdb = PQsetdbLogin("127.0.0.1","5432","","","account_db","postgres","nmmgbnmmgb");
    PQsetClientEncoding(pctx->accountdb,"UTF8");
    pctx->cmatchdb = PQsetdbLogin("127.0.0.1","5432","","","cmatch_db","postgres","nmmgbnmmgb");
    if (PQstatus(pctx->accountdb) != CONNECTION_OK) {
        fprintf(stderr, "Connection to account_db failed: %s",
                PQerrorMessage(pctx->accountdb));
        assert(0);
    }
    if (PQstatus(pctx->cmatchdb) != CONNECTION_OK) {
        fprintf(stderr, "Connection to cmatch_db failed: %s",
                PQerrorMessage(pctx->cmatchdb));
        assert(0);
    }
    
    //memcache
    const char *config_string= "--SERVER=localhost --BINARY-PROTOCOL";
    pctx->memc= memcached(config_string, strlen(config_string));
}