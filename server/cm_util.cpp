#include "cm_util.h"
#include <limits.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <assert.h>

const char *kvs_find_string(int *err, evhtp_kvs_t *kvs, const char *key)
{
    const char *str = evhtp_kv_find(kvs, key);
    if (str == NULL) {
        *err = 1;
    }
    return str;
}

int kvs_find_int(int *err, evhtp_kvs_t *kvs, const char *key)
{
    const char *str = evhtp_kv_find(kvs, key);
    if (str == NULL) {
        *err = 1;
        return 0;
    }
    char* pEnd = NULL;
    int n = strtol(str, &pEnd, 0);
    if ((n == 0 && *pEnd != '\0')
            ||(n == INT_MAX || n == INT_MIN)) {
        *err = 1;
        return 0;
    }
    return n;
}

float kvs_find_float(int *err, evhtp_kvs_t *kvs, const char *key)
{
    const char *str = evhtp_kv_find(kvs, key);
    if (str == NULL) {
        *err = 1;
        return 0.f;
    }
    char* pEnd = NULL;
    float f = strtof(str, &pEnd);
    if (*pEnd != '\0') {
        *err = 1;
        return 0.f; 
    }
    return f;
}

void unused(const void *p)
{
     
}

char *base64_cf(const char *input, int length)
{
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

char *unbase64_cf(const char *input, int length)
{
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

void sha1(sha1buf_t out, const void *input, int len)
{
    SHA_CTX s;
    SHA1_Init(&s);
    SHA1_Update(&s, input, len);
    SHA1_Final((unsigned char*)out, &s);
}

char *findCookie_cf(evhtp_request_t *req, const char *key)
{
    assert(key);
    const char *strcookie = evhtp_kv_find(req->headers_in, "Cookie");
    if (NULL == strcookie)
        return NULL;
    
    char *p = strstr(strcookie, key);
    if (NULL == p)
        return NULL;
        
    int slen = strlen(strcookie);
    char *pend = p + slen;
    p += strlen(key);
    for (; p < pend; ++p) {
        if (*p == '=') {
            int len = 0;
            char *pv = ++p;
            for (; ; ++p, ++len) {
                if ((p == pend || *p == ';') && len != 0) {
                    char *out = calloc(len+1, 1);
                    memcpy(out, pv, len);
                    return out;
                }
            }
            break;
        }
    }
    return NULL;
}
