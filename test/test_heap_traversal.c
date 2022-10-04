#include <check.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h> // tmp debug -- remove me

#include "chunk.h"
#include "dalloc_heap_traversal.h"
#include "test_heap_traversal.h"
chunk_t first;
chunk_t second;
chunk_t third;
chunk_t fourth;

void heap_traversal_tests_setup() {
	// ?todo?: auto configure N chunks with size as function of index? e.g.
	// 2 ^ (n + 1) + (-2) ^ (n)
	// 0,3
	// 1,2
	// 2,12
	// 3,8
	// 4,48

	first.in_use = true;
	first.size = 12;
	first.start = (void *)0;

	second.in_use = false;
	second.size = 96;
	second.start = (void *)1;

	third.in_use = true;
	third.size = 32;
	third.start = (void *)2;

	fourth.in_use = false;
	fourth.size = 48;
	fourth.start = (void *)3;

	first.iter = second.iter = third.iter = fourth.iter = 0;

	append(NULL, &first, &second);
	append(&first, &second, &third);
	append(&second, &third, &fourth);
}

void heap_traversal_tests_teardown() {
	
}

bool size_matches_dumb(const chunk_t *chunk, void *size) {
	return chunk->size == *(size_t *)size;
}

bool size_matches_smart(const chunk_t *chunk, void *size) {
	return !(chunk->in_use) && size_matches_dumb(chunk, size);
}

bool in_use_matches(const chunk_t *chunk, void *in_use) {
	return chunk->in_use == *(bool *)in_use;
}

chunk_t *get_nth_chunk(size_t n) {
	chunk_t *chunk = &first;
	chunk_t *prv = NULL;
	chunk_t *nxt = NULL;
	for (size_t i = 0; i < n; i++) {
		nxt = next(chunk, prv);
		prv = chunk;
		chunk = nxt;
	}
	return chunk;
}

START_TEST(test_find_i) {
	chunk_t *chunk = get_nth_chunk(_i);
	size_t size = chunk->size;
	chunk_t *prev = NULL;
	chunk_t *match = find(&first, size_matches_dumb, &size, &prev);

	// Ensure we found the correct chunk.
	ck_assert_ptr_eq(match, chunk);

	if (_i == 0) {
		// First chunk: no previous chunk: prev should be null.
		ck_assert_ptr_null(prev);
	} else {
		// Ensure the previous chunk is set correctly.
		ck_assert_ptr_nonnull(prev);
		chunk_t *actual_prev = get_nth_chunk(_i - 1);
		ck_assert_ptr_eq(prev, actual_prev);
	}
}
END_TEST

START_TEST(test_no_match) {
	chunk_t *prev = NULL;

	size_t size = 4321;
	chunk_t *match = find(&first, size_matches_dumb, &size, &prev);
	ck_assert_ptr_null(match);
	ck_assert_ptr_null(prev);
}
END_TEST

START_TEST(test_find_multiple_matches) {
	// Ensure the first matching chunk is returned by find() when
	// more than one chunk matches the given criteria.
	bool in_use = second.in_use;
	chunk_t *prev = NULL;
	chunk_t *match = find(&first, in_use_matches, &in_use, &prev);
	ck_assert_ptr_eq(&second, match);
	ck_assert_ptr_eq(&first, prev);
}
END_TEST

/*
Get the size of a chunk. This function signature is compatible with the
chunk traversal API.
*/
int32_t get_size(const chunk_t * chunk, void *user_data) {
	return chunk->size;
}

int32_t distance_from(const chunk_t *chunk, void *user_data) {
	size_t *size = (size_t *)user_data;
	return *size - chunk->size;
}

START_TEST(test_sum) {
	int32_t size_expected = first.size + second.size + third.size + fourth.size;
	int32_t size_actual = sum(&first, get_size, NULL);
	ck_assert_int_eq(size_expected, size_actual);
}
END_TEST

START_TEST(test_min) {
	chunk_t *prv = NULL;
	chunk_t *smallest = min(&first, get_size, NULL, &prv);
	ck_assert_ptr_eq(&first, smallest);
	ck_assert_ptr_eq(NULL, prv);
}
END_TEST

START_TEST(test_min_sets_prev) {
	chunk_t *expected_chunk = get_nth_chunk(_i);
	chunk_t *expected_prv = _i == 0 ? NULL : get_nth_chunk(_i - 1);

	size_t size = expected_chunk->size;
	chunk_t *prv = NULL;

	chunk_t *closest = min(&first, distance_from, &size, &prv);

	ck_assert_ptr_eq(expected_chunk, closest);
	ck_assert_ptr_eq(expected_prv, prv);
}
END_TEST

// Return the chunk's size, or -1 if the chunk is unused.
int32_t get_weight(const chunk_t *chunk, void *user_data) {
	return chunk->in_use ? chunk->size : -1;
}

START_TEST(test_min_negative_weight) {
	// Ensure that min() will ignore chunks with a negative weight.

	chunk_t *prv = NULL;
	chunk_t *smallest = min(&first, get_weight, NULL, &prv);

	ck_assert_ptr_eq(&first, smallest);
	ck_assert_ptr_eq(NULL, prv);
}
END_TEST

int32_t return_negative(const chunk_t *chunk, void *user_data) {
	return -1;
}

START_TEST(test_min_all_negative_weights) {
	// Ensure that min() returns NULL with all negative weights.
	chunk_t *prv = NULL;
	chunk_t *ch = min(&first, return_negative, NULL, &prv);

	ck_assert_ptr_null(ch);
	ck_assert_ptr_null(prv);
}
END_TEST

START_TEST(test_max) {
	// Ensure that max() works in the happy code path.
	// This should return the largest chunk (which is the 2nd chunk).
	chunk_t *prv = NULL;
	chunk_t *largest = max(&first, get_size, NULL, &prv);

	ck_assert_ptr_eq(&second, largest);
	ck_assert_ptr_eq(&first, prv);
}
END_TEST

START_TEST(test_max_negative_weight) {
	// Ensure that chunks with a negative weight are ignored. The
	// biggest chunk is not in use. get_weight() will return -1 for any
	// chunks not in use.
	chunk_t *prv = NULL;
	chunk_t *biggest_in_use_chunk = max(&first, get_weight, NULL, &prv);

	ck_assert_ptr_eq(&third, biggest_in_use_chunk);
	ck_assert_ptr_eq(&second, prv);
}
END_TEST

START_TEST(test_max_all_negative_weights) {
	// Ensure that max() returns NULL with all negative weights.
	chunk_t *prv = NULL;
	chunk_t *ch = max(&first, return_negative, NULL, &prv);

	ck_assert_ptr_null(ch);
	ck_assert_ptr_null(prv);
}
END_TEST

Suite *d_heap_traversal_test_suite() {
	Suite *suite = suite_create("heap traversal tests");

    TCase *find_tests = tcase_create("find() tests");
	tcase_add_test(find_tests, test_find_multiple_matches);
	tcase_add_test(find_tests, test_no_match);
	tcase_add_loop_test(find_tests, test_find_i, 0, 4);

	TCase *sum_tests = tcase_create("sum() tests");
	tcase_add_test(sum_tests, test_sum);

	TCase *min_tests = tcase_create("min() tests");
	tcase_add_test(min_tests, test_min);
	tcase_add_loop_test(min_tests, test_min_sets_prev, 0, 4);
	tcase_add_test(min_tests, test_min_negative_weight);
	tcase_add_test(min_tests, test_min_all_negative_weights);

	TCase *max_tests = tcase_create("max() tests");
	tcase_add_test(max_tests, test_max);
	tcase_add_loop_test(max_tests, test_max_negative_weight, 0, 4);
	tcase_add_test(max_tests, test_max_all_negative_weights);

    tcase_add_checked_fixture(find_tests, heap_traversal_tests_setup, heap_traversal_tests_teardown);
    tcase_add_checked_fixture(sum_tests, heap_traversal_tests_setup, heap_traversal_tests_teardown);
    tcase_add_checked_fixture(min_tests, heap_traversal_tests_setup, heap_traversal_tests_teardown);
    tcase_add_checked_fixture(max_tests, heap_traversal_tests_setup, heap_traversal_tests_teardown);

    suite_add_tcase(suite, find_tests);
    suite_add_tcase(suite, sum_tests);
	suite_add_tcase(suite, min_tests);
	suite_add_tcase(suite, max_tests);
    return suite;
}
