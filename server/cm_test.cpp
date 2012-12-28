#include "cm_test.h"
#include "cm_util.h"
#include <uuid/uuid.h>
#include <stdio.h>
#include <libmemcached/memcached.h>
#include <pthread.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <postgresql/libpq-fe.h>

void uuidtest() {
    uuid_t uuid;
    uuid_generate_time(uuid);

    char out[64];
    uuid_unparse(uuid, out);
    printf("uuid:%s\n", out);
}

void mutextest() {
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex,NULL);
    for ( int i = 0; i < 10000000; ++i ) {
        pthread_mutex_lock(&mutex);
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_destroy(&mutex);
}

void shatest() {
    SHA_CTX s;
    int i;
    unsigned char hash[20];
    SHA1_Init(&s);
    SHA1_Update(&s, "xxx", 3);
    SHA1_Final(hash, &s);
    for (i=0; i < 20; i++)
        printf ("%.2x", (int)hash[i]);
    printf ("\n");

    for ( int i = 0; i < 100000; ++i ) {
        char message[] = "YOYO!";
        char *b64 = base64(message, strlen(message));
        char *b = unbase64(b64, strlen(b64));
        free(b64);
        free(b);
    }

}

void dbtest() {
    PGconn *conn = PQsetdbLogin("127.0.0.1","5432","","","account_db","postgres","nmmgbnmmgb");
    //PQsetClientEncoding(m_conn,"GBK");
    if (PQstatus(conn) != CONNECTION_OK) {
        return;
    }

    PGresult *res;
    for ( int i = 0; i < 10000; ++i ) {
        res = PQexec(conn, "SELECT id FROM user_account");
        int rows = PQntuples(res);
        unused(&rows);
//        for ( int i = 0; i < rows; ++i ){
//            const char *c = PQgetvalue(res, i, 0);
//            unused(c);
//            //printf("id: %s\n", c);
//        }
    }

    PQclear(res);

    PQfinish(conn);
}

void testmemcache() {
    const char *config_string= "--SERVER=localhost --BINARY-PROTOCOL";
    memcached_st *memc= memcached(config_string, strlen(config_string));
    memcached_return_t rc = memcached_last_error(memc);
    if (rc != MEMCACHED_SUCCESS) {
        printf("memc error\n");
    }
    printf("begin\n");

    int v = 111;
    rc= memcached_set(memc, "a", 1, (const char*)&v, sizeof(int), 10000, (uint32_t)0);
    if (rc != MEMCACHED_SUCCESS) {
        printf("memcached_set error\n");
    }

    for (int i = 0; i < 10000; ++i) {
        memcached_return_t rc;
        size_t len;
        char* c = memcached_get(memc, "a", 1, &len, 0, &rc);
        if (rc != MEMCACHED_SUCCESS) {
            printf("%s\n", memcached_last_error_message(memc));
        }
        if (c) {
            int v = *(int*)c;
            unused(&v);
            free(c);
        }
    }
    printf("end\n");
    memcached_free(memc);
}


static pthread_key_t key;
static void thread_free(void *p) {
    pthread_t* t = (pthread_t*)p;
    printf("thread_free:%lu\n", *t);
    free(p);
}
static void* thread_func(void* arg) {
    printf("thread_func\n");
    pthread_t* p = (pthread_t*)malloc(sizeof(pthread_t));
    *p = pthread_self();
    if ( rand()%2 >= 0 ) {
        pthread_setspecific(key, p);
    }
    sleep(1);
    return NULL;
}

void testthr() {
    pthread_key_create(&key, thread_free);
    srand(time(NULL));
    pthread_t thr;
    for ( int i = 0; i < 8; ++i ) {
        pthread_create(&thr, NULL, thread_func, NULL);
        //void *r = NULL;
        //pthread_join(thr, &r);
    }
}

void cm_test() {
    //testlibpqxx();
    //testthr();
    //dbtest();
    //testmemcache();
    //shatest();
}
