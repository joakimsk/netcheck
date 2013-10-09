/*	Netcheck
 *	Needs libcurl
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <unistd.h> /* Requires UNIX */
#include <pthread.h>

#if !defined(__APPLE__)
#include <malloc.h>
#endif

#include "dbg.h"

#include "main.h"
#include "netcheck.h"
                                    
static int thread_terminate = 0; /* 0, 1 */

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len+1);
        if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
        }
    s->ptr[0] = '\0';
}

void termination_handler (int signum){
    debug("termination_handler() Caught signal %d\n", signum);
    main_terminate();
    netcheck_terminate();
}

void main_terminate(){
    thread_terminate = 1;
}

void join_threads(pthread_t *th1){
    debug("join_threads()\n");
    if(pthread_join(*th1, NULL)){printf("main: Error joining th1\n");}
}

int main(int argc, char *argv[]){
    int netcheck_running = 0;
    int sphinx_running = 0;
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = termination_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = SA_RESTART; /* Restart functions if interrupted
                                            by handle */
    
    if (sigaction(SIGINT, &sigIntHandler, NULL) == -1){
        printf("ERROR: SIGINT ctrl+C not set!");
    }
    
    if (sigaction(SIGTSTP, &sigIntHandler, NULL) == -1){
        printf("ERROR: SIGTSTP ctrl+Z not set!");
    }


    pthread_t th1;
	while(!thread_terminate){ /* Thread supervisor */
        if(!netcheck_get_thread_status()){
            if(pthread_create(&th1, NULL, netcheck_thread, NULL)){ 
                printf("PTHREAD: Error starting netcheck\n");
                return 2;
            }
        }
        sleep(5);
        debug("main: netcheck_get_network_status()=%d\n", netcheck_get_network_status());
    }
    
    join_threads(&th1);
    printf("Main program done.\n");
    exit(EXIT_SUCCESS);
}