#ifndef __CM_CALLBACK_H__
#define __CM_CALLBACK_H__

#include <evhtp.h>

void cm_register_cbs(evhtp_t *htp);
void cm_free_cbs();

#endif // __CM_CALLBACK_H__
