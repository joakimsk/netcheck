#ifndef NETCHECK_H_INCLUDED
#define NETCHECK_H_INCLUDED

#include <stddef.h>

/* Public */
size_t dummy_write(void *ptr, size_t size, size_t nmemb, struct string *s);
int netcheck_get_network_status();
int netcheck_get_thread_status();
void netcheck_terminate();
void* netcheck_thread(void *arg);
#endif