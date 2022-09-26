#include <unistd.h>
#include <stdio.h>

#include "dalloc.h"
#include "dalloc_internal.h"

void *malloc(size_t size) {
	fprintf(stderr, "malloc: tbi\n");
	return d_malloc(size);
}

void free(void *ptr) {
	fprintf(stderr, "free: tbi\n");
	d_free(ptr);
}

void *calloc(size_t nmemb, size_t size) {
	fprintf(stderr, "calloc: tbi\n");
	return d_calloc(nmemb, size);
}

void *realloc(void *ptr, size_t size) {
	fprintf(stderr, "realloc: tbi\n");
	return d_realloc(ptr, size);
}

void reallocarray(void *ptr, size_t nmemb, size_t size) {
	fprintf(stderr, "reallocarray: tbi\n");
	d_reallocarray(ptr, nmemb, size);
}
