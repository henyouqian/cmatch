#ifndef __CM_UTIL_H__
#define __CM_UTIL_H__

#include <evhtp.h>

#ifdef __cplusplus
extern "C" {
#endif

const char *kvs_find_string(int *err, evhtp_kvs_t *kvs, const char *key);
int kvs_find_int(int *err, evhtp_kvs_t *kvs, const char *key);
float kvs_find_float(int *err, evhtp_kvs_t *kvs, const char *key);

void unused(const void *p);

char *base64_cf(const char *input, int length);    //caller free
char *unbase64_cf(const char *input, int length);  //caller free

typedef char sha1buf_t[16];
void sha1(sha1buf_t out, const void *input, int len);

char *findCookie(evhtp_request_t *req, const char *key);

//log
#define lwinfo(fmt, args...) do{printf("*i|"); printf(fmt, ##args); printf("|@%s\n", __FUNCTION__);}while(0)
#define lwerror(fmt, args...) do{printf("*e|"); printf(fmt, ##args); printf("|@%s\n", __FUNCTION__);}while(0)
#define lwassert(_e) assert(_e);

#ifdef __cplusplus
} //extern "C"
#endif

#endif // __CM_UTIL_H__
