#ifndef _DALLOC_CHUNK_H_
#define _DALLOC_CHUNK_H_

typedef struct {
	void *start;
	size_t size;
	void *next;
} chunk_t;

#endif // _DALLOC_CHUNK_H_
