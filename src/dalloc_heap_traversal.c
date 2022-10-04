#include <stddef.h>

#include "dalloc_heap_traversal.h"

chunk_t *find(chunk_t *start, predicate_t condition, void *user_data, chunk_t **prev) {
	chunk_t *chunk = start;
	while (chunk) {
		if (condition(chunk, user_data)) {
			return chunk;
		}
		chunk_t *nxt = next(chunk, *prev);
		*prev = chunk;
		chunk = nxt;
	}

	// Clear prev before returning.
	*prev = NULL;
	return NULL;
}

int32_t sum(chunk_t *start, aggregator_t aggregator, void *user_data) {
	int32_t total = 0;
	chunk_t *chunk = start;
	chunk_t *prv = NULL;
	while (chunk) {
		total += aggregator(chunk, user_data);
		chunk_t *nxt = next(chunk, prv);
		prv = chunk;
		chunk = nxt;
	}
	return total;
}

chunk_t *max(chunk_t *start, aggregator_t weight, void *user_data, chunk_t **prev) {
	chunk_t *max = NULL;
	chunk_t *max_prev = NULL;
	int32_t max_weight = INT32_MIN;

	chunk_t *chunk = start;
	while (chunk) {
		int32_t chunk_weight = weight(chunk, user_data);
		if (chunk_weight >= 0 && chunk_weight > max_weight) {
			max = chunk;
			max_prev = *prev;
			max_weight = chunk_weight;
		}
		chunk_t *nxt = next(chunk, *prev);
		*prev = chunk;
		chunk = nxt;
	}

	*prev = max_prev;
	return max;
}

chunk_t *min(chunk_t *start, aggregator_t weight, void *user_data, chunk_t **prev) {
	chunk_t *min_chunk = NULL;
	chunk_t *min_prev = NULL;
	int32_t min_weight = INT32_MAX;

	chunk_t *chunk = start;
	while (chunk) {
		int32_t chunk_weight = weight(chunk, user_data);
		if (chunk_weight >= 0 && chunk_weight < min_weight) {
			min_chunk = chunk;
			min_prev = *prev;
			min_weight = chunk_weight;
		}

		chunk_t *nxt = next(chunk, *prev);
		*prev = chunk;
		chunk = nxt;
	}

	*prev = min_prev;
	return min_chunk;
}
