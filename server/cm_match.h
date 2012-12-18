#ifndef __MATCH_H__
#define __MATCH_H__

#include <evhtp.h>

void register_match_cb(evhtp_t *htp);
void free_match_cb();

#endif // __MATCH_H__
