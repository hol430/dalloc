#include <assert.h>
#include <check.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "dalloc_io.h"
#include "dalloc_internal.h"
#include "test_free.h"

void free_tests_setup() {
	set_log_level(DALLOC_LOG_LEVEL_NONE);
}

void free_tests_teardown() {
	
}

void allocate_and_free(size_t size) {
}

START_TEST(ensure_single_chunk_is_released) {
	// Allocate a single chunk, then free it. Ensure that this memory is
	// released to the OS. NOTE: This behaviour is debatable but this is
	// more for demonstrative purposes anyway.

	// Get current position of program break.
	void *pbrk0 = sbrk(0);

    unsigned char* ptr = d_malloc(_i);
    ck_assert_ptr_nonnull(ptr);

	// Get position of program break after the allocation. This should be
	// greater than it was before the allocation.
	void *pbrk1 = sbrk(0);

    d_free(ptr);

	// Get the position of program break again. It should have returned
	// to its initial position.
	void *pbrk2 = sbrk(0);

	// Assertions.
	ck_assert_ptr_ne(pbrk0, pbrk1);
	ck_assert_ptr_eq(pbrk0, pbrk2);
}
END_TEST

START_TEST(free_without_allocating) {
	// This should crash; that expectation is encapsulated in adding
	// this test into the test case.
	int x;
	d_free(&x);
}
END_TEST

START_TEST(test_greedy_free) {
	// Allocate two chunks, then free them in the reverse order and
	// ensure that all of the allocated memory is reclaimed properly.
	void *pbrk_initial = sbrk(0);

	void *ptr0 = d_malloc(8);
	ck_assert_ptr_nonnull(ptr0);

	void *ptr1 = d_malloc(8);
	ck_assert_ptr_nonnull(ptr1);

	void *pbrk0 = sbrk(0);

	// Free the first chunk. This won't actually be released to the OS
	// yet, because the second chunk (which is closer to the program
	// break) is still in use.
	d_free(ptr0);
	void *pbrk1 = sbrk(0);
	ck_assert_ptr_eq(pbrk0, pbrk1);

	// Free the second chunk. At this point, the program break should
	// be as it was before any allocations occurred.
	d_free(ptr1);
	void *pbrk2 = sbrk(0);
	ck_assert_ptr_eq(pbrk_initial, pbrk2);
}
END_TEST

Suite *d_free_test_suite() {
	Suite* suite;
    TCase* test_case;

    suite = suite_create("free tests");
    test_case = tcase_create("d_free test case");

    tcase_add_loop_test(test_case, ensure_single_chunk_is_released, 1, 32);
    tcase_add_checked_fixture(test_case, free_tests_setup, free_tests_teardown);
	tcase_add_test(test_case, test_greedy_free);
#if CK_FORK != no
	tcase_add_test_raise_signal(test_case, free_without_allocating, SIGILL);
#endif
    suite_add_tcase(suite, test_case);
    return suite;
}
