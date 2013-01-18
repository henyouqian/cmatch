#include <stdio.h>
#include <evhtp.h>
#include <event2/event.h>
#include "cm_callback.h"
#include "cm_test.h"
#include "cm_util.h"
#include "cm_context.h"

const char *bind_addr = "0.0.0.0";
uint16_t bind_port = 8081;

void *input_thread(void *arg) {
    while (1) {
        char string [256];
        fgets(string, sizeof(string), stdin);
        if (strcmp(string, "exit\n") == 0) {
            exit(0);
        } else {
            printf("what???\n");
        }
    }
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t thread;
    pthread_create(&thread, NULL, input_thread, NULL);
    
    cm_test();
    
    evbase_t *evbase = event_base_new();
    evhtp_t *htp = evhtp_new(evbase, NULL);
    cm_register_cbs(htp);
    
    if (evhtp_bind_socket(htp, bind_addr, bind_port, 128) < 0) {
        fprintf(stderr, "Could not bind socket: %s\n", strerror(errno));
        exit(-1);
    }
    
    //evhtp_use_threads(htp, cm_thread_init_cb, g_thread_nums, NULL);
    lwinfo("server start");
    event_base_loop(evbase, 0);
    
    cm_free_cbs();
    evhtp_free(htp);
    event_base_free(evbase);
    pthread_cancel(thread);
    pthread_cancel(cm_get_context()->tid);
	return 0;
}
