#ifndef __CM_UTIL_H__
#define __CM_UTIL_H__

#include <evhtp.h>
#include <vector>
#include <stdint.h>
#include <string>

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

struct MemIO{
    char* p0;
    char* p;
    int capacity;
    bool overflow;
    
    MemIO();
    MemIO(char* ptr, int capacity);
    ~MemIO();
    void set(char* ptr, int capacity);
    int remain();
    int length();
    
    void            write(const void *src, int nbytes); //return writed data ptr, so you can set value later
    void            writeChar(char v);
    void            writeUchar(unsigned char v);
    void            writeShort(short v);
    void            writeUshort(unsigned short v);
    void            writeInt(int v);
    void            writeUint(unsigned int v);
    void            writeInt64(int64_t v);
    void            writeUint64(uint64_t v);
    void            writeFloat(float v);
    void            writeString(const char *str);
    void            printf(const char *fmt, ...);
    
    void            read(void* buf, unsigned int nbytes);
    void*           read(unsigned int nbytes);
    char            readChar();
    unsigned char   readUchar();
    short           readShort();
    unsigned short  readUshort();
    int             readInt();
    unsigned int    readUint();
    int64_t         readInt64();
    uint64_t        readUint64();
    float           readFloat();
    char*           readString();
};

int32_t     s2int32(const char* str, int* err = NULL);
uint32_t    s2uint32(const char* str, int* err = NULL);
int64_t     s2int64(const char* str, int* err = NULL);
uint64_t    s2uint64(const char* str, int* err = NULL);
float       s2float(const char* str, int* err = NULL);
double      s2double(const char* str, int* err = NULL);

class CommaReader {
public:
    CommaReader(const char* str);
    int getStatus();
    int readInt(int &out);
    int readFloat(float &out);
    int readString(std::string &out);
    
    enum {
        err_null_ptr = -1,
        err_already_err = -2,
        err_empty_section = -3,
        err_too_long = -4,
        err_type_convert = -5,
        status_end = 1,
    };
    
private:
    const char *_p;
    int _status;    //0:ok <0:error 1:end
};

#endif // __CM_UTIL_H__
