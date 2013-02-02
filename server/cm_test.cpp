#include "cm_test.h"
#include "cm_util.h"
#include <uuid/uuid.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <postgresql/libpq-fe.h>
#include <memory>
#include <string>
#include <hiredis/hiredis.h>

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
        char *b64 = base64_cf(message, strlen(message));
        size_t len;
        char *b = (char*)unbase64_cf(b64, &len);
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

void threadtest() {
    pthread_key_create(&key, thread_free);
    srand(time(NULL));
    pthread_t thr;
    for ( int i = 0; i < 8; ++i ) {
        pthread_create(&thr, NULL, thread_func, NULL);
        //void *r = NULL;
        //pthread_join(thr, &r);
    }
}

void redistest() {
    redisContext *c = redisConnect("127.0.0.1", 6379);
    if (c->err) {
        printf("Error: %s\n", c->errstr);
        return;
    }
    
    char buf[1024];
    MemIO mio;
    mio.set(buf, sizeof(buf));
    mio.writeInt(345);
    mio.writeInt(111);
    mio.printf("liwei%dwuhaili%.2f", 34, 2.23f);
    mio.writeFloat(23.43f);
    mio.writeInt64(345834854389532);
    
    redisReply *reply = (redisReply*)redisCommand(c, "set a %b", mio.p0, mio.p-mio.p0);
    freeReplyObject(reply);
    
    reply = (redisReply*)redisCommand(c, "get a");
    char *p = (reply->str);
    mio.set(p, reply->len);
    int a = mio.readInt();
    a = mio.readInt();
    const char* b = mio.readString();
    float d = mio.readFloat();
    int64_t e = mio.readInt64();
    lwinfo("%d, %s, %f, %lld", a, b, d, e);
    freeReplyObject(reply);
    
    int age = 31;
    reply = (redisReply*)redisCommand(c, "HSET myhmap age %b", &age, 4);
    freeReplyObject(reply);
    
    reply = (redisReply*)redisCommand(c, "HGET myhmap age");
    mio.set(reply->str, reply->len);
    age = mio.readInt();
    lwinfo("age:%i", age);
    freeReplyObject(reply);
    
    redisFree(c);
}

void cm_test() {
    //redistest();
    redisReply *reply = NULL;
    Autofree _af(reply, freeReplyObject);
}
