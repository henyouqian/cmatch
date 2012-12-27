#include "cm_util.h"
#include <limits.h>
#include <openssl/sha.h>
#include <openssl/bio.h>

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

char *base64(const char *input, int length) {
    BIO *bmem, *b64;
    BUF_MEM *bptr;
    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);
    char *buff = (char *)malloc(bptr->length);
    memcpy(buff, bptr->data, bptr->length-1);
    buff[bptr->length-1] = 0;
    BIO_free_all(b64);
    return buff;
}

char *unbase64(const char *input, int length) {
    BIO *b64, *bmem;
    char *buffer = (char *)malloc(length);
    memset(buffer, 0, length);
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bmem = BIO_new_mem_buf((void*)input, length);
    bmem = BIO_push(b64, bmem);
    BIO_read(bmem, buffer, length);
    BIO_free_all(bmem);
    return buffer;
}

void sha1(sha1buf_t out, const void *input, int len){
    SHA_CTX s;
    SHA1_Init(&s);
    SHA1_Update(&s, input, len);
    SHA1_Final((unsigned char*)out, &s);
}