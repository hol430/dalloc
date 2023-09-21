#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "chunk.h"
#include "dalloc.h"
#include "dalloc_io.h"
#include "dalloc_utils.h"
#include "dalloc_config.h"

typedef struct {
	chunk_t *start;
	chunk_t *tail;
} heap_t;

heap_t heap;

void *_sbrk(intptr_t increment) {
	return sbrk(increment);
}

void *d_malloc(size_t size) {
	if (size == 0) {
		// As mandated by the spec.
		return (void *)0;
	}

	// Attempt to find an unused chunk on the hepa.
	chunk_t *found = find_unused_chunk_first(heap.start, size);
	if (found) {
		found->in_use = true;
		return found->start;
	}

	// The amount of storage required for the chunk + metadata.
	size_t actual_size = size + sizeof(chunk_t);

	void *allocated = _sbrk(actual_size);
	if (allocated == (void *)-1) {
		// Allocation error. ERRNO is set by sbrk.
		// Let the caller determine how this should be handled.
		return 0;
	}

	// Bookkeeping.
	chunk_t *chunk = (chunk_t*)allocated;
	chunk->start = allocated + sizeof(chunk_t);
	chunk->size = size;
	chunk->in_use = true;

	if (!heap.start) {
		// This is the first block of memory allocated by this process.
		heap.start = chunk;
		heap.tail = chunk;
		chunk->iter = 0;
	} else {
		// Put this chunk on the end of the list.
		append(prev(heap.tail, NULL), heap.tail, chunk);
		heap.tail = chunk;
	}

	// Return the address of user-writable memory.
	return chunk->start;
}

void d_free(void *ptr) {
	if (!heap.start) {
		// User error. Undefined behaviour.
		panic("Attempted to free memory without first allocating");
		return;
	}

	chunk_t *prv = NULL;
	chunk_t *chunk = find_chunk(heap.start, ptr, &prv);
	if (!chunk) {
		// User error. Either a double-free or just passing in garbage.
		// Either way, undefined behaviour is allowed by the spec.
		panic("free() error: invalid pointer");
		return;
	}

	if (!chunk->in_use) {
		panic("free(): double free or corrupted heap");
		return;
	}

	chunk->in_use = false;

	// todo: coalesce nearby unused chunks.

	while (heap.tail && !heap.tail->in_use) {
		chunk_t *freed_chunk = heap.tail;
		if (heap.tail == heap.start) {
			heap.tail = heap.start = NULL;
		} else {
			chunk_t *previous = prev(heap.tail, NULL);
			remove_after(previous, freed_chunk);
			heap.tail = previous;
		}

		// The amount of space occupied by this chunk and its metadata.
		size_t to_free = sizeof(chunk_t) + freed_chunk->size;

		// Release the memory back to the OS.
		void *res = _sbrk(-to_free);

		if (res == (void *)-1) {
			// If this failed, it's probably a bug in our code.
			// Let's pretend like nothing is wrong for now...
			log_warning("Failed to free() memory. Likely a dalloc bug");
			size_t alloc = total_allocated(heap.start);
			log_diag("Attempted to free %d bytes. Total allocated = %d.", to_free, alloc);
			panic("d_free(): heap corruption");
		}
	}
}

void *d_calloc(size_t nmemb, size_t size) {
	size_t total = nmemb * size;
	if (total / nmemb != size) {
		// Integer overflow.
		log_warning("Allocating %d elements of size %d results in integer overflow", nmemb, size);
		return NULL;
	}
	void *ptr = d_malloc(total);

	if (!ptr) { 
		return NULL;
	}

	size_t nchar = total / sizeof(char);
	for (size_t i = 0; i < nchar; i++) {
		((char *)ptr)[i] = 0;
	}
	return ptr;
}

void *d_realloc(void *ptr, size_t size) {
	if (!ptr) {
		// If ptr is NULL, then the call is equivalent to malloc(size), for all
		// values of size.
		return d_malloc(size);
	}

	if (size == 0 && ptr) {
		// For compatibility with glibc malloc, if size is equal to zero, and
		// ptr is not NULL, then the call is equivalent to free(ptr). Note that
		// this is not required by the posix spec.
		d_free(ptr);
		return NULL;
	}

	if (!heap.start) {
		// User error. Undefined behaviour.
		panic("realloc() error: Attempted to realloc memory without first allocating");
		return NULL;
	}

	chunk_t *prv = NULL;
	chunk_t *chunk = find_chunk(heap.start, ptr, &prv);
	if (!chunk) {
		// User error. Either a double-free or just passing in garbage.
		// Either way, undefined behaviour is allowed by the spec.
		panic("realloc() error: invalid pointer");
		return NULL;
	}

	if (!chunk->in_use) {
		panic("realloc(): attempted to realloc previously freed memory, or heap is corrupt");
		return NULL;
	}

	if (size == chunk->size) {
		// realloc() to same size.
		return chunk->start;
	}

	if (size > chunk->size) {
		// We need a bigger chunk. We could attempt to coalsece nearby blocks,
		// but for now, let's just handle this the old-fashioned way.
		// todo: this will be extremely inefficient if the old ptr is at the top
		// of the heap.
		chunk = NULL;
		void *new_ptr = d_malloc(size);
		for (size_t i = 0; i < size; i++) {
			((char *)new_ptr)[i] = ((char *)ptr)[i];
		}
		d_free(ptr);
		return new_ptr;
	}

	// We want a smaller chunk.
	size_t remainder = chunk->size - size;
	if (remainder <= sizeof(chunk_t)) {
		// The current chunk is slightly larger than the required size, and the
		// difference is less than the minimum required to store a header for a
		// new chunk. Therefore just return the pointer to the current chunk.
		// todo: improve this (e.g. what if current chunk is at top of heap?)
		return ptr;
	}

	// Reduce the current chunk to the requested size.
	chunk->size = size;

	// Create a new unused chunk with the remaining space.
	chunk_t *new_chunk = chunk->start + size;
	new_chunk->size = remainder - sizeof(chunk_t);;
	new_chunk->in_use = false;
	new_chunk->start = ((void *)new_chunk) + sizeof(chunk_t);
	append(prv, chunk, new_chunk);

	return chunk->start;
}

void *d_reallocarray(void *ptr, size_t nmemb, size_t size) {
	size_t total = nmemb * size;
	if (total / nmemb != size) {
		// Integer overflow.
		log_warning("reallocarray(): Allocating %d elements of size %d results in integer overflow", nmemb, size);
		return NULL;
	}
	return d_realloc(ptr, total);
}
