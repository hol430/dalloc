#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "dalloc_heap_traversal.h"
#include "dalloc_utils.h"

/*
Check whether the specified chunk is not in use and has the capacity to store
a certain amount of data (specified by user_data).

@param chunk: The chunk.
@param user_data: A pointer to size_t, specifying the minimum required size.
*/
bool can_store(const chunk_t *chunk, void *user_data) {
	return !(chunk->in_use) && chunk->size >= *(size_t *)user_data;
}

/*
Check whether the specified address (user_mem) is the start address of
the user-writable memory of the specified chunk.

@param chunk: The chunk.
@param user_mem: Start address of the user-writable memory.
*/
bool is_chunk(const chunk_t *chunk, void *user_mem) {
	return chunk->start == user_mem;
}

chunk_t *find_chunk(chunk_t *start, void *user_mem, chunk_t **prev) {
	return find(start, is_chunk, user_mem, prev);
}

chunk_t *find_unused_chunk_first(chunk_t *start, size_t size) {
	chunk_t *prev = NULL;
	return find(start, can_store, &size, &prev);
}

/*
Return the difference between the size of the chunk and the size
specified by user_data.

@param chunk: The chunk.
@param user_data: Pointer to size_t. The size used for comparison.
*/
int32_t chunk_size_difference(const chunk_t *chunk, void *user_data) {
	if (chunk->in_use) {
		return -1;
	}
	return chunk->size - *(size_t *)user_data;
}

chunk_t *find_unused_chunk_bestfit(chunk_t *start, size_t size) {
	chunk_t *prev = NULL;
	return min(start, chunk_size_difference, &size, &prev);
}

int32_t get_allocation(const chunk_t *chunk, void *user_data) {
	return chunk->size + sizeof(chunk_t);
}

size_t total_allocated(chunk_t* start) {
	return sum(start, get_allocation, NULL);
}

bool is_contiguous(chunk_t *x, chunk_t *y) {
	return x->start + x->size == y;
}
