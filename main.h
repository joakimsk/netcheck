#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stddef.h>

/* Public */
struct string {
    char *ptr;
    size_t len;
};

/* Private */
void main_terminate();
void init_string(struct string *s);
void termination_handler (int signum);
size_t dummy_write(void *ptr, size_t size, size_t nmemb, struct string *s);
#endif