#include <stddef.h>
#include <stdint.h>

#include "chunk.h"

void *xor(void *x, void *y) {
	if (!x) {
		return y;
	}
	if (!y) {
		return x;
	}
	return (void *)( (uintptr_t)x ^ (uintptr_t)y );
}

void *calculate_iter(chunk_t *x, chunk_t* y) {
	return xor( (void *)x, (void *)y );
}

chunk_t *next(chunk_t *chunk, chunk_t *prev) {
	return (chunk_t *)xor(chunk->iter, prev);
}

chunk_t *prev(chunk_t *chunk, chunk_t *next_chunk) {
	return (chunk_t *)xor(chunk->iter, next_chunk);
}

void append(chunk_t *prv, chunk_t *chunk, chunk_t *appendee) {
	if (!prv) {
		// Chunk is at start of list. Need to modify this chunk's iter
		// as well as that of the "next" chunk - ie the chunk which is
		// currently the second chunk but which will become the third.
		chunk_t *nxt = next(chunk, NULL);
		if (nxt) {
			chunk_t *third = next(nxt, chunk);

			chunk->iter = (void *)appendee;
			appendee->iter = calculate_iter(chunk, nxt);
			nxt->iter = calculate_iter(appendee, third);
		} else {
			// Chunk is the only element in the list.
			chunk->iter = (void *)appendee;
			appendee->iter = (void *)chunk;
		}
	} else if (chunk->iter == (void *)prv) {
		// Chunk is at end of list - only need to modify the current
		// end of list (and the new chunk itself of course).
		chunk->iter = calculate_iter(prv, appendee);
		appendee->iter = (void *)chunk;
	} else {
		// Chunk is somewhere in the middle of the list, so we'll need
		// to modify the chunks before and after the newly-inserted chunk.
		chunk_t *next_chunk = next(chunk, prv);
		chunk_t *next_next = next(next_chunk, chunk);

		appendee->iter = calculate_iter(chunk, next_chunk);
		chunk->iter = calculate_iter(prv, appendee);
		next_chunk->iter = calculate_iter(next_next, appendee);
	}
}

void remove_after(chunk_t *prev_chunk, chunk_t *chunk) {
	if (!prev_chunk) {
		// Chunk is at start of list. Only need to modify the next chunk.
		chunk_t *nxt = next(chunk, NULL);
		if (nxt) {
			chunk_t *third = next(nxt, chunk);

			// Third is now the 2nd chunk in the list.
			nxt->iter = (void *)third;
		} else {
			// Chunk is the only element in the list. We can't remove it
			// from itself. Therefore this is a noop.
		}
	} else if (chunk->iter == (void *)prev_chunk) {
		// Chunk is at the end of the list - only need to modify prev.
		// iter ^ next yields prev
		prev_chunk->iter = xor(prev_chunk->iter, chunk);
		chunk->iter = NULL;
	} else {
		// Chunk is somewhere in the middle of the list. Need to modify
		// elements before and after.
		chunk_t *prev_prev = prev(prev_chunk, chunk);
		chunk_t *next_chunk = next(chunk, prev_chunk);
		chunk_t *next_next = next(next_chunk, chunk);
	
		prev_chunk->iter = calculate_iter(prev_prev, next_chunk);
		next_chunk->iter = calculate_iter(next_next, prev_chunk);
	}
}

void remove_before(chunk_t *next_chunk, chunk_t *chunk) {
	chunk_t *prev_chunk = next_chunk ? prev(chunk, next_chunk) : NULL;
	remove_after(prev_chunk, chunk);
}
