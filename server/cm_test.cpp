#include "cm_test.h"
#include "cm_util.h"
#include <uuid/uuid.h>
#include <stdio.h>
#include <libmemcached/memcached.h>
#include <pthread.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <postgresql/libpq-fe.h>

void uuidtest(){
    uuid_t uuid;
    uuid_generate_time(uuid);
    
    char out[64];
    uuid_unparse(uuid, out);
    printf("uuid:%s\n", out);
}

void mutextest(){
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex,NULL);
    for ( int i = 0; i < 10000000; ++i ){
        pthread_mutex_lock(&mutex);
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_destroy(&mutex);
}

void shatest(){
    SHA_CTX s;
    int i;
    //int size;
    //char c[512];
    unsigned char hash[20];
    SHA1_Init(&s);
//    while ((size=::read (0, c, 512)) > 0)
//        SHA1_Update(&s, c, size);

    SHA1_Update(&s, "xxx", 3);
    SHA1_Final(hash, &s);
    for (i=0; i < 20; i++)
        printf ("%.2x", (int)hash[i]);
    printf ("\n");
}

void dbtest(){
    PGconn *conn = PQsetdbLogin("127.0.0.1","5432","","","account","postgres","nmmgbnmmgb");
    //PQsetClientEncoding(m_conn,"GBK");
    if (PQstatus(conn) != CONNECTION_OK){
        return;
    }
    
    PGresult *res;
    for ( int i = 0; i < 10000; ++i ){
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

void cm_test(){
    printf("begin test\n");
    //dbtest();
}
