#ifndef _DALLOC_UTIL_H_
#define _DALLOC_UTIL_H_

#include "chunk.h"

/*
Find a chunk in a chunk list, return 0 if not found.

@param start: Start point of the search.
@param user_mem: User memory start address of the chunk for which we're searching.
@param prev: This will be set to the address of the previous chunk in the list. If no chunk or the 1st chunk is found, this will be NULL.
*/
chunk_t *find_chunk(chunk_t *start, void *user_mem, chunk_t **prev);

/*
Count the total number of allocated bytes after the given starting point.

@param start: Starting point of the search.
*/
size_t total_allocated(chunk_t* start);

#endif // _DALLOC_UTIL_H_
