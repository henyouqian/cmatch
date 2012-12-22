#include <stdio.h>
#include <evhtp.h>
#include <event2/event.h>
#include "cm_callback.h"

const char *bind_addr = "0.0.0.0";
uint16_t bind_port = 8081;
const int thread_nums = 8;

int main(int argc, char **argv)
{
    evbase_t *evbase = event_base_new();
    evhtp_t *htp = evhtp_new(evbase, NULL);
    cm_register_cbs(htp);
    
    if (evhtp_bind_socket(htp, bind_addr, bind_port, 128) < 0) {
        fprintf(stderr, "Could not bind socket: %s\n", strerror(errno));
        exit(-1);
    }
    
    evhtp_use_threads(htp, NULL, thread_nums, NULL);
    event_base_loop(evbase, 0);
    
    cm_unregister_cbs();
    evhtp_free(htp);
    event_base_free(evbase);
    
	return 0;
}
