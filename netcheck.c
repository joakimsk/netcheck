#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h> /* libcurl web */

#include "dbg.h"

#include "main.h"
#include "netcheck.h"


static int network_status = -1; /* -1, 0, 1, 2 */
static int thread_status = 0; /* 0, 1 */
static int thread_terminate = 0; /* 0, 1 */

size_t dummy_write(void *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len+1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    return size*nmemb;
}

int netcheck_get_network_status(){
    return network_status;
}

int netcheck_get_thread_status(){
    return thread_status;
}

void netcheck_terminate(){
    thread_terminate = 1;
}

void* netcheck_thread(void *arg){
    thread_status = 1;
	CURL *curl;
    CURLcode res;

    int *incoming = (int *)arg;
    debug("netcheck: Thread started\n");

    while(!thread_terminate){
        struct string s;
        init_string(&s);

        curl = curl_easy_init();
        if(curl) {
                curl_easy_setopt(curl, CURLOPT_URL, "http://google.com");
                curl_easy_setopt(curl, CURLOPT_HEADER, "0"); /* Do not return header */
                curl_easy_setopt(curl, CURLOPT_NOBODY, "1"); /* Do not return body */
                curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, "1"); /* Disable Nagle's Algorithm */
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L); /* TIMEOUT */
                curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L); /* To prevent stack jump errors */
                curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1L); /* TIMEOUT */
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &dummy_write); /* Function that handles returned */
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s); /* Send all returned data here */
                
                res = curl_easy_perform(curl);

                /* Check for errors */
                if(res != CURLE_OK){
                    //printf("netcheck:\tInternet=FALSE\n");
                    network_status = 0;
                    //fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                } else {
                    //printf("netcheck:\tInternet=TRUE\n");

                    char *location_str;
                    if(strstr(s.ptr, "Location:") == NULL){
                        //printf("netcheck: Location-tag not found\n");
                    } else {
                        location_str = strstr(s.ptr, "Location:");
                        if (strncmp(location_str, "Location: http://www.google.com", 31) == 0){
                            //printf("netcheck:\tGoogle-access=TRUE\n");
                            network_status = 2;
                        } else {
                            //printf("netcheck:\tGoogle-access=FALSE\n");
                            network_status = 1;
                        }
                    }
                }
                
                free(s.ptr);
                curl_easy_cleanup(curl);
        } else {
            printf("netcheck: CURL died, terminate the ship!");
            thread_terminate = 1;
        }
        sleep(1);
    }
	debug("netcheck terminated\n");
	thread_status = 0;
    return NULL;
}
