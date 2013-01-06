#include "cm_buf.h"
#include <algorithm>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

MemIO::MemIO(){
    p = p0 = NULL;
    capacity = 0;
    overflow = false;
}

MemIO::~MemIO(){
    
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

