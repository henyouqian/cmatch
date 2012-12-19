#ifndef __CM_UTIL_H__
#define __CM_UTIL_H__

#include <evhtp.h>

class CmKvs{
public:
    CmKvs(evhtp_request_t *req);
    const char* findString(const char *key);
    int findInt(const char *key);
    float findFloat(const char *key);
    bool hasError();
    
private:
    bool _hasError;
    evhtp_query_t* _kvs;
};

#endif // __CM_UTIL_H__
