#ifndef __CM_UTIL_H__
#define __CM_UTIL_H__

#include <evhtp.h>
#include <vector>

const char *kvs_find_string(int *err, evhtp_kvs_t *kvs, const char *key);
int kvs_find_int(int *err, evhtp_kvs_t *kvs, const char *key);
float kvs_find_float(int *err, evhtp_kvs_t *kvs, const char *key);

void unused(const void *p);

char *base64_cf(const void *input, size_t length);
void *unbase64_cf(const char *input, size_t *length);

typedef char sha1buf_t[16];
void sha1(sha1buf_t out, const void *input, int len);

char *findCookie_cf(evhtp_request_t *req, const char *key); //caller free

//log
#define lwinfo(fmt, args...) do{printf("*i|"); printf(fmt, ##args); printf("|@%s\n", __FUNCTION__);}while(0)
#define lwerror(fmt, args...) do{printf("*e|"); printf(fmt, ##args); printf("|@%s\n", __FUNCTION__);}while(0)
#define lwassert(_e) assert(_e);

//autofree
typedef void (*Freefunc) (void*);
class Autofree {
public:
    Autofree(void* p, Freefunc func = ::free);
    ~Autofree();
    void free();
    
private:
    void *_p;
    Freefunc _freeFunc;
};

//memory pool
class Mempool {
public:
    Mempool(int objSize, int objsPerChunk);
    ~Mempool();
    void* newObj();
    void delObj(void* pObj);
    int getObjSize();
    
private:
    void newChunk();
    std::vector<char*> _chunks;
    std::vector<void*> _freelist;
    int _objSize;
    int _objsPerChunk;
};

template<typename _Tp, int objsPerChunk>
class PoolAllocator : public std::allocator<_Tp> {
public:
    typedef size_t     size_type;
    typedef _Tp*       pointer;
    
    PoolAllocator()
    :_mpool(sizeof(_Tp), objsPerChunk) {
    }
    ~PoolAllocator() {
    }
    pointer allocate(size_type __n, const void* = 0) {
        if (__n != 1)
            std::__throw_bad_alloc();
        return (pointer) _mpool.newObj();
    }
    
    void deallocate(pointer __p, size_type) {
        _mpool.delObj(__p);
    }
private:
    Mempool _mpool;
};

#endif // __CM_UTIL_H__
