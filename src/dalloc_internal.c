#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include "chunk.h"
#include "dalloc_internal.h"
#include "dalloc_io.h"
#include "dalloc_utils.h"

typedef struct {
	chunk_t *start;
	chunk_t *tail;
} state_t;

state_t heap;

void *d_malloc(size_t size) {
	if (size == 0) {
		// As mandated by the spec.
		return (void *)0;
	}

	// The amount of storage required for the chunk + metadata.
	size_t actual_size = size + sizeof(chunk_t);

	void *allocated = sbrk(actual_size);
	if (allocated == (void *)-1) {
		// Allocation error. ERRNO is set by sbrk.
		// Let the caller determine how this should be handled.
		return 0;
	}

	// Bookkeeping.
	chunk_t *chunk = (chunk_t*)allocated;
	chunk->start = allocated + sizeof(chunk_t);
	chunk->size = size;
	chunk->next = NULL;

	if (!heap.start) {
		// This is the first block of memory allocated by this process.
		heap.start = chunk;
		heap.tail = chunk;
	} else {
		// Put this chunk on the end of the list.
		heap.tail->next = chunk;
		heap.tail = chunk;
	}

	// Return the address of user-writable memory.
	return chunk->start;
}

void d_free(void *ptr) {
	if (!heap.start) {
		// User error. Undefined behaviour.
		panic("Attempted to free memory without first allocating\n");
	}

	chunk_t *prev = NULL;
	chunk_t *chunk = find_chunk(heap.start, ptr, &prev);
	if (!chunk) {
		// User error. Either a double-free or just passing in garbage.
		// Either way, undefined behaviour is allowed.
		panic("Invalid free()\n");
	}

	// tbi: coalesce nearby unused chunks.

	if (chunk == heap.tail) {
		// The user has freed the chunk at the end of the list.
		// Let's free it.
		// todo: think about holding onto chunks...

		// First we will remove the chunk from the list.
		if (!prev) {
			// This is the only chunk in the list.
			heap.start = NULL;
			heap.tail = NULL;
		} else {
			prev->next = NULL;
			heap.tail = prev;
		}

		// The amount of space occupied by this chunk and its metadata.
		size_t to_free = sizeof(chunk_t) + chunk->size;

		// Release the memory back to the OS.
		void *res = sbrk(-to_free);

		if (res == (void *)-1) {
			// If this failed, it's probably a bug in our code.
			// Let's pretend like nothing is wrong for now...
			log_warning("Failed to free() memory. Likely a dalloc bug");
			size_t alloc = total_allocated(heap.start);
			log_diag("Attempted to free %d bytes. Total allocated = %d.", to_free, alloc);
		}
	}
}

void *d_calloc(size_t nmemb, size_t size) {
	panic("d_calloc tbi");
	return NULL;
}

void *d_realloc(void *ptr, size_t size) {
	panic("d_realloc tbi");
	return NULL;
}

void d_reallocarray(void *ptr, size_t nmemb, size_t size) {
	panic("d_reallocarray tbi");
}

