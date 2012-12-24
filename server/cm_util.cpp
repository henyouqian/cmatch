#include "cm_util.h"
#include <limits.h>

CmKvs::CmKvs(evhtp_kvs_t *kvs)
:_hasError(false), _kvs(kvs){
    
}

const char* CmKvs::findString(const char *key){
    const char *str = evhtp_kv_find(_kvs, key);
    if ( str == NULL ){
        _hasError = true;
    }
    return str;
}

int CmKvs::findInt(const char *key){
    const char *str = evhtp_kv_find(_kvs, key);
    if ( str == NULL ){
        _hasError = true;
        return 0;
    }
    char* pEnd = NULL;
    int n = strtol(str, &pEnd, 0);
    if ( (n == 0 && *pEnd != '\0')
        ||(n == INT_MAX || n == INT_MIN)){
        _hasError = true;
        return 0;
    }
    return n;
}

float CmKvs::findFloat(const char *key){
    const char *str = evhtp_kv_find(_kvs, key);
    if ( str == NULL ){
        _hasError = true;
        return 0.f;
    }
    char* pEnd = NULL;
    float f = strtof(str, &pEnd);
    if ( *pEnd != '\0' ){
        _hasError = true;
        return 0.f; 
    }
    return f;
}

bool CmKvs::hasError(){
    return _hasError;
}

void unused(const void *p){
    
}