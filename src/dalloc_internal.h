#ifndef _DALLOC_INTERNAL_H_
#define _DALLOC_INTERNAL_H_

#include <stddef.h>

void *d_malloc(size_t size);
void d_free(void *ptr);
void *d_calloc(size_t nmemb, size_t size);
void *d_realloc(void *ptr, size_t size);
void d_reallocarray(void *ptr, size_t nmemb, size_t size);

#endif // _DALLOC_INTERNAL_H_
