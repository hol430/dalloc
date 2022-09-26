#ifndef _DALLOC_H_
#define _DALLOC_H_

#include <stddef.h>

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void reallocarray(void *ptr, size_t nmemb, size_t size);

#endif // _DALLOC_H_
