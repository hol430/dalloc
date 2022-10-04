#ifndef _DALLOC_HEAP_TRAVERSAL_H_
#define _DALLOC_HEAP_TRAVERSAL_H_

#include <stdint.h>

#include "chunk.h"

typedef bool (*predicate_t)(const chunk_t *, void *user_data);
typedef int32_t (*aggregator_t)(const chunk_t *, void *user_data);

/*
Find the first in the specified heap which matches a condition. Returns
NULL if no matching chunk is found.

@param start: Starting point of the search.
@param condition: The condition used to select a chunk.
@param user_data: User data which will be passed to the predicate.
@param prev: (out parameter): Will be set to the previous chunk in the
		     heap, or NULL if the first chunk is returned.
*/
chunk_t *find(chunk_t *start, predicate_t condition, void *user_data, chunk_t **prev);

/*
Return the sum of a given function over all chunks in the heap.

@param start: The start point of the sum.
@param aggregator: Function which returns a value for each chunk.
@param user_data: User data which will be passed to the aggregator.
*/
int32_t sum(chunk_t *start, aggregator_t aggregator, void *user_data);

/*
Return the chunk in the heap with the maximum value given by the weight
function.

@param start: Starting point of the search.
@param weight: Function which returns a weighting for each chunk. A
			   negative return value means the chunk will be ignored.
@param user_data: User data to be passed to the weighting function.
@param prev: (out parameter): will be set to the previous chunk in the
			 heap, or NULL if the first chunk is returned.
*/
chunk_t *max(chunk_t *start, aggregator_t weight, void *user_data, chunk_t **prev);

/*
Return the chunk in the heap with the minimum value given by the weight
function.

@param start: Starting point of the search.
@param weight: Function which returns a weighting for each chunk. A
			   neative return value means the chunk will be ignored.
@param user_data: User data to be passed to the weighting function.
@param prev: (out parameter): will be set to the previous chunk in the
			 heap, or NULL if the first chunk is returned.
*/
chunk_t *min(chunk_t *start, aggregator_t weight, void *user_data, chunk_t **prev);

#endif // _DALLOC_HEAP_TRAVERSAL_H_
