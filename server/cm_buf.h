#ifndef __CM_BUF_H__
#define __CM_BUF_H__

#include <stdint.h>

struct MemIO{
    char* p0;
    char* p;
    int capacity;
    bool overflow;
    
    MemIO();
    ~MemIO();
    void set(char* ptr, int capacity);
    int remain();
    
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

#endif // __CM_BUF_H__
