#include <stdio.h>

#include "dalloc_utils.h"


chunk_t *find_chunk(chunk_t *start, void *user_mem, chunk_t **prev) {
	*prev = NULL;
	for (chunk_t *chunk = start; chunk != NULL; chunk = chunk->next) {
		if (chunk->start == user_mem) {
			return chunk;
		}
		*prev = chunk;
	}
	return NULL;
}

size_t total_allocated(chunk_t* start) {
	size_t count = 0;
	for (chunk_t *chunk = start; chunk != NULL; chunk = chunk->next) {
		count += chunk->size + sizeof(chunk_t);
	}
	return count;
}
