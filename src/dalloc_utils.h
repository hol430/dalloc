#ifndef _DALLOC_UTIL_H_
#define _DALLOC_UTIL_H_

#include <stdbool.h>
#include <stddef.h>

#include "chunk.h"

/*
Find metadata for a particular chunk in the heap, return 0 if not
found.

@param start: Start point of the search.
@param user_mem: Start address of the chunk's user-writable memory of
				 the chunk for which we're searching.
@param prev: This will be set to the address of the previous chunk in
			 the list. If no chunk or the 1st chunk is found, this will
			 be NULL.
*/
chunk_t *find_chunk(chunk_t *start, void *user_mem, chunk_t **prev);

/*
Find the first unused chunk which is greater than or equal to the given
size. Return 0 if none found.

@param start: The starting point of the search.
@param size: Desired size of the chunk.
*/
chunk_t *find_unused_chunk_first(chunk_t *start, size_t size);

/*
Find the chunk which is unused and closest in size to the required
size.

@param start: Start point of the search.
@param size: Desired size of the chunk.
*/
chunk_t *find_unused_chunk_bestfit(chunk_t *start, size_t size);

/*
Count the total number of allocated bytes after the given starting point.

@param start: Starting point of the search.
*/
size_t total_allocated(chunk_t* start);

/*
Check if two chunks are contiguous.

@param x: The first chunk.
@param y: The second chunk.
*/
bool is_contiguous(chunk_t *x, chunk_t *y);

#endif // _DALLOC_UTIL_H_
