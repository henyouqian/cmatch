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

static unsigned char encoding_table[] = {   '+', '/', '0', '1', '2', '3', '4', '5', 
                                            '6', '7', '8', '9', 'A', 'B', 'C', 'D', 
                                            'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 
                                            'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
                                            'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 
                                            'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 
                                            'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 
                                            's', 't', 'u', 'v', 'w', 'x', 'y', 'z',  };
static unsigned char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};

class decoding_table_builder{
public:
    decoding_table_builder(){
        decoding_table = (unsigned char*)malloc(256);
        for (int i = 0; i < 0x40; i++)
            decoding_table[encoding_table[i]] = i;
    }
    ~decoding_table_builder(){
        free(decoding_table);
    }
};

decoding_table_builder _decoding_table_builder;

char *base64_cf(const void *indata,
                    size_t input_length) {
    const char *data = (const char*)indata;
    size_t output_length = (size_t) (4.0 * ceil((double) input_length / 3.0));

    unsigned char *encoded_data = (unsigned char*)malloc(output_length+1);
    if (encoded_data == NULL) return NULL;
    encoded_data[output_length] = 0;
    
    for (size_t i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[output_length - 1 - i] = '=';

    return (char*)encoded_data;
}


void *unbase64_cf(const char *indata,
                    size_t *output_length) {
    size_t input_length = strlen(indata);
    const unsigned char *data = (const unsigned char*)indata;

    if (input_length % 4 != 0) return NULL;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    char *decoded_data = (char*)malloc(*output_length+1);
    decoded_data[*output_length] = 0;
    if (decoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
                        + (sextet_b << 2 * 6)
                        + (sextet_c << 1 * 6)
                        + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return decoded_data;
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

void Autofree::free() {
    if (_p) {
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
    for (int i; i < _objsPerChunk; ++i) {
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