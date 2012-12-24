#include "cm_test.h"
#include <uuid/uuid.h>
#include <stdio.h>
#include <libmemcached/memcached.h>
#include <pthread.h>

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

void cm_test(){
    mutextest();
}
