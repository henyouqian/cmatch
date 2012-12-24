#ifndef __CM_UTIL_H__
#define __CM_UTIL_H__

#include <evhtp.h>

class CmKvs{
public:
    CmKvs(evhtp_kvs_t *kvs);
    const char* findString(const char *key);
    int findInt(const char *key);
    float findFloat(const char *key);
    bool hasError();
    
private:
    bool _hasError;
    evhtp_kvs_t* _kvs;
};

void unused(const void *p);

#endif // __CM_UTIL_H__
