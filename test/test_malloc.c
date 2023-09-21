#include <stdio.h>
#include <assert.h>
#include <check.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>

#include "dalloc_io.h"
#include "dalloc.h"
#include "test_util.h"

void malloc_tests_setup() {
	set_log_level(DALLOC_LOG_LEVEL_NONE);
}

void malloc_tests_teardown() {
	
}

START_TEST(allocate_zero) {
    void *res = d_malloc(0);
    ck_assert_ptr_eq(res, (void *)0);
}
END_TEST

START_TEST(allocate_2n) {
    unsigned char *ptr = d_malloc(pow(2, _i));

    // Attempt to write to this memory. Granted this isn't a great way
    // to check if it's actually writable...
    for (size_t i = 0; i < _i; i++) {
        ptr[i] = 0;
    }

    d_free(ptr);
}
END_TEST

START_TEST(allocate) {
    unsigned char* ptr = d_malloc(_i);

    ck_assert_ptr_nonnull(ptr);

    // Attempt to write to this memory.
    for (size_t i = 0; i < _i; i++) {
        ptr[i] = 0;
    }

    d_free(ptr);
}
END_TEST

void *custom_malloc_sbrk(intptr_t ptr) {
    return (void *)-1;
}

START_TEST(sbrk_failure) {
    attach_sbrk_handler(custom_malloc_sbrk);
    void *ptr = d_malloc(8);
    remove_sbrk_handlers();
    ck_assert_ptr_eq(NULL, ptr);
}
END_TEST

START_TEST(test_malloc_no_chunk_exists) {
    // Ensure that memory is acquired from the OS when no other chunks
    // exist in the heap.
    void *pbrk0 = sbrk(0);
    size_t request_size = 16;
    unsigned char *allocated = d_malloc(request_size);
    void *pbrk1 = sbrk(0);

    // Make sure that enough memory has been allocated.
    ck_assert((uintptr_t)pbrk1 >= (uintptr_t)pbrk0 + request_size);

    // Let's try to write to this memory. It's not a great test, but
    // what can you do...
    for (size_t i = 0; i < request_size; i++) {
        allocated[i] = 0;
    }

    // Release the memory.
    d_free(allocated);
}
END_TEST

START_TEST(test_malloc_used_chunk_exists) {
    // Allocate twice. Ensure that memory is acquired from the OS both
    // times.
    size_t size = 64;

    // Get current position of program break.
    void *pbrk0 = sbrk(0);

    void *allocations[_i];
    for (int32_t i = 0; i < _i; i++) {
        allocations[i] = d_malloc(size);
        void *pbrki = sbrk(0);
        ck_assert((uintptr_t)pbrki >= (uintptr_t)pbrk0 + (i + 1) * size);
    }

    for (int32_t i = 0; i < _i; i++) {
        d_free(allocations[i]);
    }
    void *pbrk_end = sbrk(0);
    ck_assert_ptr_eq(pbrk_end, pbrk0);
}
END_TEST

START_TEST(test_malloc_unused_chunk_exists) {
	// Ensure that an unused chunk will be reused if possible, rather
	// than allocating more memory.

	// First allocate two chunks, then free the first. The first one
	// can't be released to the OS at this time. Then allocate another
	// chunk of the same size or smaller, and we should get the first
	// chunk back (and no additional memory should be requested from
	// the OS).
	size_t size = 8;
	void *ptr0 = d_malloc(size);
	void *ptr1 = d_malloc(16);

	// Won't be released to the OS.
	void *pbrk0 = sbrk(0);
	d_free(ptr0);

	for (size_t s = 0; s < size; s++) {
		void *ptr2 = d_malloc(size);
		ck_assert_ptr_eq(ptr0, ptr2);
		ck_assert_ptr_eq(pbrk0, sbrk(0));
		d_free(ptr2);
	}

    d_free(ptr1);
}
END_TEST

Suite *d_malloc_test_suite() {
    TCase* test_case = tcase_create("malloc Test Case");
    tcase_add_checked_fixture(test_case, malloc_tests_setup, malloc_tests_teardown);

    tcase_add_loop_test(test_case, allocate, 1, 8);
    tcase_add_loop_test(test_case, allocate_2n, 4, 16);
    tcase_add_test(test_case, allocate_zero);
    tcase_add_test(test_case, test_malloc_no_chunk_exists);
    tcase_add_test(test_case, test_malloc_unused_chunk_exists);
    tcase_add_loop_test(test_case, test_malloc_used_chunk_exists, 1, 10);
    tcase_add_test(test_case, sbrk_failure);

	Suite* suite = suite_create("malloc Tests");
    suite_add_tcase(suite, test_case);
    return suite;
}
