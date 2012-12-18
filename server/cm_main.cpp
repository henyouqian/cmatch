#include <stdio.h>
#include <evhtp.h>
#include <event2/event.h>
#include "match.h"

const char *bind_addr = "0.0.0.0";
uint16_t bind_port = 8081;

int main(int argc, char **argv)
{
    evbase_t *evbase = event_base_new();
    evhtp_t *htp = evhtp_new(evbase, NULL);
    register_match_cb(htp);
    
    if (evhtp_bind_socket(htp, bind_addr, bind_port, 128) < 0) {
        fprintf(stderr, "Could not bind socket: %s\n", strerror(errno));
        exit(-1);
    }
    event_base_loop(evbase, 0);
    
    free_match_cb();
    evhtp_free(htp);
    event_base_free(evbase);
    
	return 0;
}
