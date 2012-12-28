#ifndef __CM_UTIL_H__
#define __CM_UTIL_H__

#include <evhtp.h>

//kvs
class CmKvs{
public:
    CmKvs(evhtp_kvs_t *kvs);
    const char *findString(const char *key);
    int findInt(const char *key);
    float findFloat(const char *key);
    bool hasError();
    
private:
    bool _hasError;
    evhtp_kvs_t *_kvs;
};

void unused(const void *p);

char *base64(const char *input, int length);    //must free returned pointer
char *unbase64(const char *input, int length);  //must free returned pointer

typedef char sha1buf_t[16];
void sha1(sha1buf_t out, const void *input, int len);

//log
#define lwinfo(fmt, args...) do{printf("*i|"); printf(fmt, ##args); printf("|@%s\n", __FUNCTION__);}while(0)
#define lwerror(fmt, args...) do{printf("*e|"); printf(fmt, ##args); printf("|@%s\n", __FUNCTION__);}while(0)
#define lwassert(_e) assert(_e);

#endif // __CM_UTIL_H__
