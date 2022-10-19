#include <check.h>
#include <stdint.h>

#include "dalloc.h"
#include "dalloc_utils.h"

static chunk_t first;
static chunk_t second;
static chunk_t third;
static chunk_t fourth;

void utils_tests_setup() {
	// ?todo?: auto configure N chunks with size as function of index? e.g.
	// 2 ^ (n + 1) + (-2) ^ (n)
	// 0,3
	// 1,2
	// 2,12
	// 3,8
	// 4,48

	first.in_use = true;
	first.size = 16;
	first.start = (void *)0;

	second.in_use = false;
	second.size = 32;
	second.start = (void *)1;

	third.in_use = true;
	third.size = 64;
	third.start = (void *)2;

	fourth.in_use = false;
	fourth.size = 128;
	fourth.start = (void *)3;

	first.iter = second.iter = third.iter = fourth.iter = 0;

	append(NULL, &first, &second);
	append(&first, &second, &third);
	append(&second, &third, &fourth);
}

void utils_tests_teardown() {

}

START_TEST(test_find_unused_bestfit_larger_unused_exists) {
	// Test the bestfit find function when the only chunk in the heap
	// large enough size is unused.
	chunk_t *result = find_unused_chunk_bestfit(&first, 96);
	ck_assert_ptr_eq(&fourth, result);
}
END_TEST

START_TEST(test_find_unused_bestfit_larger_used_unused_exists) {
	// Test the bestfit find function when both used and unused chunks
	// with enough size exist in the heap.
	chunk_t *result = find_unused_chunk_bestfit(&first, 48);
	ck_assert_ptr_eq(&fourth, result);
}
END_TEST

START_TEST(test_find_unused_exact_exists) {
	// Ensure that bestfit will return a chunk with the exact size
	// required when no other larger chunks exist.
	chunk_t *result = find_unused_chunk_bestfit(&first, 128);
	ck_assert_ptr_eq(&fourth, result);
}
END_TEST

START_TEST(test_find_unused_exact_exists_larget_exist) {
	// Ensure that bestfit will return a chunk with the exact size
	// required when larger chunks do exist.
	chunk_t *result = find_unused_chunk_bestfit(&first, 32);
	ck_assert_ptr_eq(&second, result);
}
END_TEST

START_TEST(test_find_unused_smaller_exists) {
	// Ensure that bestfit returns NULL when unused chunks exist but
	// all are too small.
	chunk_t *result = find_unused_chunk_bestfit(&first, 1024);
	ck_assert_ptr_null(result);
}
END_TEST

START_TEST(test_find_unused_smaller_used_exists) {
	// Ensure that bestfit returns NULL when no unused chunks exist.
	second.in_use = fourth.in_use = true;
	chunk_t *result = find_unused_chunk_bestfit(&first, 8);
	ck_assert_ptr_null(result);
}
END_TEST

START_TEST(test_find_unused_bestfit_closest_in_size) {
	// Ensure that bestfit find will actually return the closest chunk
	// in size (not the biggest!).
	chunk_t *result = find_unused_chunk_bestfit(&first, 24);
	ck_assert_ptr_eq(&second, result);
}
END_TEST

START_TEST(test_total_allocated_happy_path) {
	int32_t expected = first.size + second.size + third.size + fourth.size + 4 * sizeof(chunk_t);
	int32_t actual = total_allocated(&first);
	ck_assert_int_eq(expected, actual);
}
END_TEST

START_TEST(test_is_contiguous) {
	void *ptr0 = d_malloc(8);
	void *ptr1 = d_malloc(16);
	void *ptr2 = d_malloc(32);

	// This will break if we change the way that we use the allocated space.
	chunk_t *ch0 = (chunk_t *)(ptr0 - sizeof(chunk_t));
	chunk_t *ch1 = (chunk_t *)(ptr1 - sizeof(chunk_t));
	chunk_t *ch2 = (chunk_t *)(ptr2 - sizeof(chunk_t));

	ck_assert(is_contiguous(ch0, ch1));
	ck_assert(is_contiguous(ch1, ch2));
	ck_assert(!is_contiguous(ch0, ch2));

	d_free(ptr0);
	d_free(ptr1);
	d_free(ptr2);
}
END_TEST

Suite *d_utils_test_suite() {
	Suite* suite;
    TCase* test_case;

    suite = suite_create("utils tests");
    test_case = tcase_create("d_utils test case");
	tcase_add_checked_fixture(test_case, utils_tests_setup, utils_tests_teardown);
    suite_add_tcase(suite, test_case);

	tcase_add_test(test_case, test_find_unused_bestfit_larger_unused_exists);
	tcase_add_test(test_case, test_find_unused_bestfit_larger_used_unused_exists);
	tcase_add_test(test_case, test_find_unused_exact_exists);
	tcase_add_test(test_case, test_find_unused_exact_exists_larget_exist);
	tcase_add_test(test_case, test_find_unused_smaller_exists);
	tcase_add_test(test_case, test_find_unused_smaller_used_exists);
	tcase_add_test(test_case, test_find_unused_bestfit_closest_in_size);
	tcase_add_test(test_case, test_total_allocated_happy_path);
	tcase_add_test(test_case, test_is_contiguous);

    return suite;
}
