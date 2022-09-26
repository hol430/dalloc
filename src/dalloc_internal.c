#include <stddef.h>
#include <unistd.h>

#include "dalloc_internal.h"

void *d_malloc(size_t size) {
	_exit(1);
}

void d_free(void *ptr) {
	_exit(1);
}

void *d_calloc(size_t nmemb, size_t size) {
	_exit(1);
}

void *d_realloc(void *ptr, size_t size) {
	_exit(1);
}

void d_reallocarray(void *ptr, size_t nmemb, size_t size) {
	_exit(1);
}

