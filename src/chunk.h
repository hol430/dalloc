#ifndef _DALLOC_CHUNK_H_
#define _DALLOC_CHUNK_H_

#include <stdbool.h>

/*
I've implemented the heap as an XOR linked list for now.
*/
typedef struct {
	void *start;
	size_t size;
	void *iter;
	bool in_use;
} chunk_t;

/*
Return the next chunk in the heap.

@param chunk: Return the chunk after this one.
@param prev: The previous chunk (ie the chunk before `chunk`). May be
			 null if `chunk` is the first chunk.
*/
chunk_t *next(chunk_t *chunk, chunk_t *prev);

/*
Return the previous chunk in the heap.

@param chunk: Return the chunk before this one.
@param next: The next chunk (ie the chunk after `chunk`). May be null
			 if `chunk` is the last chunk.
*/
chunk_t *prev(chunk_t *chunk, chunk_t *next);

/*
Append a chunk to the heap.

@param prev: The previous chunk in the heap. May be null if chunk is
			 the first chunk.
@param chunk: The chunk will be appended to this chunk.
@param appendee: The chunk to be appended to `chunk`.
*/
void append(chunk_t *prev, chunk_t *chunk, chunk_t *appendee);

/*
Remove a chunk from the heap.

@param prev: The previous chunk in the heap (ie the chunk before
			 `chunk`). May be null if `chunk` is the first chunk.
@param chunk: The chunk to be removed from the heap.
*/
void remove_after(chunk_t *prev, chunk_t *chunk);

/*
Remove a chunk from the heap.

@param next: The next chunk in the heap (ie the chunk after `chunk`).
			 May be null if `chunk` is the last chunk.
@param chunk: The chunk to be removed from the heap.
*/
void remove_before(chunk_t *next, chunk_t *chunk);

#endif // _DALLOC_CHUNK_H_
