#include "cm_util.h"
#include <limits.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <assert.h>
#include <math.h>

const char *kvs_find_string(int *err, evhtp_kvs_t *kvs, const char *key) {
    const char *str = evhtp_kv_find(kvs, key);
    if (str == NULL) {
        *err = 1;
    }
    return str;
}

int kvs_find_int(int *err, evhtp_kvs_t *kvs, const char *key) {
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

float kvs_find_float(int *err, evhtp_kvs_t *kvs, const char *key) {
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

void unused(const void *p) {
     
}






const char encodeCharacterTable[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char decodeCharacterTable[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
    ,-1,62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21
    ,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
    ,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1
};
char *base64_cf(const void *in, size_t input_length) {
    unsigned char *input = (unsigned char*)in;
    char buff1[3];
    char buff2[4];
    unsigned char i=0, j;
    unsigned input_cnt=0;
    unsigned output_cnt=0;
    
    size_t output_length = (size_t) (4.0 * ceil((double) input_length / 3.0));
    unsigned char *output = (unsigned char*)malloc(output_length+1);
    if (output == NULL) return NULL;
    output[output_length] = 0;
    
    while(input_cnt<input_length) {
        buff1[i++] = input[input_cnt++];
        if (i==3) {
            output[output_cnt++] = encodeCharacterTable[(buff1[0] & 0xfc) >> 2];
            output[output_cnt++] = encodeCharacterTable[((buff1[0] & 0x03) << 4) + ((buff1[1] & 0xf0) >> 4)];
            output[output_cnt++] = encodeCharacterTable[((buff1[1] & 0x0f) << 2) + ((buff1[2] & 0xc0) >> 6)];
            output[output_cnt++] = encodeCharacterTable[buff1[2] & 0x3f];
            i=0;
        }
    }
    if (i) {
        for(j=i; j<3; j++) {
            buff1[j] = '\0';
        }
        buff2[0] = (buff1[0] & 0xfc) >> 2;
        buff2[1] = ((buff1[0] & 0x03) << 4) + ((buff1[1] & 0xf0) >> 4);
        buff2[2] = ((buff1[1] & 0x0f) << 2) + ((buff1[2] & 0xc0) >> 6);
        buff2[3] = buff1[2] & 0x3f;
        for (j=0; j<(i+1); j++) {
            output[output_cnt++] = encodeCharacterTable[(int)buff2[j]];
        }
        while(i++<3) {
            output[output_cnt++] = '=';
        }
    }
    return (char*)output;
}
void *unbase64_cf(const char *input, size_t *output_length) {
    char buff1[4];
    char buff2[4];
    unsigned char i=0, j;
    unsigned input_cnt=0;
    unsigned output_cnt=0;
    
    size_t input_length = strlen(input);
    if (input_length % 4 != 0) return NULL;
    *output_length = input_length / 4 * 3;
    if (input[input_length - 1] == '=') (*output_length)--;
    if (input[input_length - 2] == '=') (*output_length)--;

    char *output = (char*)malloc(*output_length+1);
    output[*output_length] = 0;
    if (output == NULL) return NULL;
    
    while(input_cnt<input_length) {
        buff2[i] = input[input_cnt++];
        if (buff2[i] == '=') {
            break;
        }
        if (++i==4) {
            for (i=0; i!=4; i++) {
                buff2[i] = decodeCharacterTable[(int)buff2[i]];
            }
            output[output_cnt++] = (char)((buff2[0] << 2) + ((buff2[1] & 0x30) >> 4));
            output[output_cnt++] = (char)(((buff2[1] & 0xf) << 4) + ((buff2[2] & 0x3c) >> 2));
            output[output_cnt++] = (char)(((buff2[2] & 0x3) << 6) + buff2[3]);
            i=0;
        }
    }
    if (i) {
        for (j=i; j<4; j++) {
            buff2[j] = '\0';
        }
        for (j=0; j<4; j++) {
            buff2[j] = decodeCharacterTable[(int)buff2[j]];
        }
        buff1[0] = (buff2[0] << 2) + ((buff2[1] & 0x30) >> 4);
        buff1[1] = ((buff2[1] & 0xf) << 4) + ((buff2[2] & 0x3c) >> 2);
        buff1[2] = ((buff2[2] & 0x3) << 6) + buff2[3];
        for (j=0; j<(i-1); j++) {
            output[output_cnt++] = (char)buff1[j];
        }
    }
    return output;
}






void sha1(sha1buf_t out, const void *input, int len) {
    SHA_CTX s;
    SHA1_Init(&s);
    SHA1_Update(&s, input, len);
    SHA1_Final((unsigned char*)out, &s);
}

char *findCookie_cf(evhtp_request_t *req, const char *key) {
    assert(key);
    const char *strcookie = evhtp_kv_find(req->headers_in, "Cookie");
    if (NULL == strcookie)
        return NULL;
    
    char *p = (char*)strstr(strcookie, key);
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
                    char *out = (char*)calloc(len+1, 1);
                    memcpy(out, pv, len);
                    return out;
                }
            }
            break;
        }
    }
    return NULL;
}

Autofree::Autofree(void* p, Freefunc func)
:_p(p), _freeFunc(func) {
    
}

Autofree::~Autofree() {
    free();
}

void Autofree::set(void* p, Freefunc func) {
    free();
    _p = p;
    _freeFunc = func;
}

void Autofree::free() {
    if (_p && _freeFunc) {
        _freeFunc(_p);
        _p = NULL;
    }
}

Mempool::Mempool(int objSize, int objsPerChunk)
:_objSize(objSize), _objsPerChunk(objsPerChunk) {
    newChunk();
    _freelist.reserve(objsPerChunk);
}

Mempool::~Mempool() {
    std::vector<char*>::iterator it = _chunks.begin();
    std::vector<char*>::iterator itend = _chunks.end();
    for (; it != itend; ++it) {
        delete [] (*it);
    }
}

void Mempool::newChunk() {
    int chunkSize = _objSize * _objsPerChunk;
    char *pChunk = new char[chunkSize];
    _chunks.push_back(pChunk);
    char *p = pChunk;
    for (int i = 0; i < _objsPerChunk; ++i) {
        _freelist.push_back(p);
        p += _objSize;
    }
}

void* Mempool::newObj() {
    if (_freelist.empty()) {
        newChunk();
    }
    void *p = _freelist.back();
    _freelist.pop_back();
    return p;
}

void Mempool::delObj(void* pBlock) {
    _freelist.push_back(pBlock);
}

int Mempool::getObjSize() {
    return _objSize;
}

MemIO::MemIO(){
    p = p0 = NULL;
    capacity = 0;
    overflow = false;
}

MemIO::~MemIO(){
    
}

MemIO::MemIO(char* ptr, int _capacity) {
    p = p0 = ptr;
    capacity = _capacity;
}

void MemIO::set(char* ptr, int _capacity){
    p = p0 = ptr;
    capacity = _capacity;
}

int MemIO::remain(){
    int remain = p0+capacity-p;
    remain = std::max(0, remain);
    return remain;
}

int MemIO::length(){
    return p - p0;
}

void MemIO::write(const void *src, int size){
    assert(size > 0);
    if ( p + size > p0 + capacity ){
        overflow = true;
        return;
    }
    if ( src ){
        memcpy(p, src, size);
    }
    p += size;
}

void MemIO::writeChar(char v){
    write(&v, sizeof(v));
}

void MemIO::writeUchar(unsigned char v){
    write(&v, sizeof(v));
}

void MemIO::writeShort(short v){
    write(&v, sizeof(v));
}

void MemIO::writeUshort(unsigned short v){
    write(&v, sizeof(v));
}

void MemIO::writeInt(int v){
    write(&v, sizeof(v));
}

void MemIO::writeUint(unsigned int v){
    write(&v, sizeof(v));
}

void MemIO::writeInt64(int64_t v){
    write(&v, sizeof(v));
}

void MemIO::writeUint64(uint64_t v){
    write(&v, sizeof(v));
}

void MemIO::writeFloat(float f){
    int n;
    memcpy(&n, &f, 4);
    write(&n, sizeof(n));
}

void MemIO::writeString(const char *str){
    assert(str);
    int size = strlen(str)+1;
    write(str, size);
}

void MemIO::printf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int n = vsnprintf(p, remain(), format, ap);
    va_end(ap);
    p += n+1;
    if (p >= p0 + capacity)
        overflow = true;
}

void MemIO::read(void* buf, unsigned int nbytes){
    assert(nbytes > 0 && buf);
    if ( p + nbytes > p0 + capacity ){
        return;
    }
    memcpy(buf, p, nbytes);
    p += nbytes;
}

void* MemIO::read(unsigned int nbytes){
    assert(nbytes > 0);
    if ( p + nbytes > p0 + capacity ){
        return NULL;
    }
    void *pp = p;
    p += nbytes;
    return pp;
}

char MemIO::readChar(){
    char* p = (char*)read(sizeof(char));
    return *p;
}

unsigned char MemIO::readUchar(){
    unsigned char* p = (unsigned char*)read(sizeof(unsigned char));
    return *p;
}

short MemIO::readShort(){
    short* p = (short*)read(sizeof(short));
    return *p;
}

unsigned short MemIO::readUshort(){
    unsigned short* p = (unsigned short*)read(sizeof(unsigned short));
    return *p;
}

int MemIO::readInt(){
    int* p = (int*)read(sizeof(int));
    return *p;
}

unsigned int MemIO::readUint(){
    unsigned int* p = (unsigned int*)read(sizeof(unsigned int));
    return *p;
}

int64_t MemIO::readInt64(){
    int64_t* p = (int64_t*)read(sizeof(int64_t));
    return *p;
}

uint64_t MemIO::readUint64(){
    uint64_t* p = (uint64_t*)read(sizeof(uint64_t));
    return *p;
}

float MemIO::readFloat(){
    float f;
    int n = readInt();
    memcpy(&f, &n, 4);
    return f;
}

char* MemIO::readString(){
    int len = strlen(p)+1;
    char *p= (char*)read(len);
    return p;
}

#define _check_str_null    if (str == NULL) {           \
                                    if (err)            \
                                        *err = 1;       \
                                    return 0;           \
                                }

int32_t s2int32(const char* str, int* err) {
    _check_str_null
    char* pEnd = NULL;
    int32_t n = strtol(str, &pEnd, 0);
    if ((*str != '\0' && *pEnd == '\0')) {
        return n;
    } else {
        if (err)
            *err = 1;
        return 0;
    }
}

uint32_t s2uint32(const char* str, int* err) {
    _check_str_null
    char* pEnd = NULL;
    uint32_t n = strtoul(str, &pEnd, 0);
    if ((*str != '\0' && *pEnd == '\0')) {
        return n;
    } else {
        if (err)
            *err = 1;
        return 0;
    }
}

int64_t s2int64(const char* str, int* err) {
    _check_str_null
    char* pEnd = NULL;
    int64_t n = strtoll(str, &pEnd, 0);
    if ((*str != '\0' && *pEnd == '\0')) {
        return n;
    } else {
        if (err)
            *err = 1;
        return 0;
    }
}

uint64_t s2uint64(const char* str, int* err) {
    _check_str_null
    char* pEnd = NULL;
    uint64_t n = strtoull(str, &pEnd, 0);
    if ((*str != '\0' && *pEnd == '\0')) {
        return n;
    } else {
        if (err)
            *err = 1;
        return 0;
    }
}

float s2float(const char* str, int* err) {
    _check_str_null
    char* pEnd = NULL;
    float n = strtof(str, &pEnd);
    if ((*str != '\0' && *pEnd == '\0')) {
        return n;
    } else {
        if (err)
            *err = 1;
        return 0.f;
    }
}

double s2double(const char* str, int* err) {
    _check_str_null
    char* pEnd = NULL;
    double n = strtod(str, &pEnd);
    if ((*str != '\0' && *pEnd == '\0')) {
        return n;
    } else {
        if (err)
            *err = 1;
        return 0.0;
    }
}

CommaReader::CommaReader(const char* str)
:_p(str), _status(0) {
    if (_p == NULL)
        _status = err_null_ptr;
}

int CommaReader::getStatus() {
    return _status;
}

int CommaReader::readInt(int &out) {
    if (_status)
        return _status = err_already_err;
        
    const char* p0 = _p;
    while (1) {
        if (*_p == 0 || *_p == ',')
            break;
        ++_p;
    }
    if (_p == p0)
        return _status = err_empty_section;
        
    char buf[32] = {0};
    if (_p-p0 >= (int)sizeof(buf))
        return _status = err_too_long;
    
    memcpy(buf, p0, _p-p0);
    int err = 0;
    out = s2int32(buf, &err);
    if (err)
        return _status = err_type_convert;
    
    if (*_p == 0)
        _status = status_end;
    else if (*_p == ',')
        ++_p;
        
    return 0;
}

int CommaReader::readFloat(float &out) {
    if (_status)
        return _status = err_already_err;
        
    const char* p0 = _p;
    while (1) {
        if (*_p == 0 || *_p == ',')
            break;
        ++_p;
    }
    if (_p == p0)
        return _status = err_empty_section;
        
    char buf[32] = {0};
    if (_p-p0 >= (int)sizeof(buf))
        return _status = err_too_long;
    
    memcpy(buf, p0, _p-p0);
    int err = 0;
    out = s2float(buf, &err);
    if (err)
        return _status = err_type_convert;
    
    if (*_p == 0)
        _status = status_end;
    else if (*_p == ',')
        ++_p;
        
    return 0;
}

int CommaReader::readString(std::string &out) {
    if (_status)
        return _status = err_already_err;
        
    const char* p0 = _p;
    while (1) {
        if (*_p == 0 || *_p == ',')
            break;
        ++_p;
    }
    if (_p == p0)
        return _status = err_empty_section;
        
    char buf[32] = {0};
    if (_p-p0 >= (int)sizeof(buf))
        return _status = err_too_long;
    
    memcpy(buf, p0, _p-p0);
    out = buf;
    
    if (*_p == 0)
        _status = status_end;
    else if (*_p == ',')
        ++_p;
        
    return 0;
}

Profiler::Profiler() {
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    _t = ts.tv_sec+ts.tv_nsec*0.000000001;
}

Profiler::~Profiler() {
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    double t = ts.tv_sec+ts.tv_nsec*0.000000001;
    lwinfo("%f", (float)(t-_t));
}