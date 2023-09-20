#include <assert.h>
#include <check.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "dalloc.h"
#include "dalloc_io.h"
#include "test_free.h"
#include "test_util.h"

bool _test_free_sigill_raised;

void *custom_sbrk(intptr_t increment) {
	sbrk(increment);
	return (void *)-1;
}

void free_tests_setup() {
	set_log_level(DALLOC_LOG_LEVEL_NONE);
	_test_free_sigill_raised = false;
}

void free_tests_teardown() {

}

void _test_free_sigill_handler(int signum) {
	ck_assert_int_eq(SIGILL, signum);
	_test_free_sigill_raised = true;
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

START_TEST(test_free_noalloc) {
	// Attempt to free an invalid pointer without first allocating anything.
	// This should result in a crash.
	attach_signal_handler(SIGILL, _test_free_sigill_handler);

	ck_assert_int_eq(false, _test_free_sigill_raised);
	// This should fail on the basis that we haven't yet allocated anything.
	int x;
	d_free(&x);

	ck_assert_int_eq(true, _test_free_sigill_raised);

	detach_signal_handlers(SIGILL);
}
END_TEST

START_TEST(test_free_invalid) {
	// Attempt to free an invalid pointer after allocating something. This
	// should result in a crash.
	attach_signal_handler(SIGILL, _test_free_sigill_handler);

	ck_assert_int_eq(false, _test_free_sigill_raised);

	// Initialise the heap by allocating something.
	void *p = d_malloc(32);

	// This should fail on the basis that it's an invalid pointer.
	int x;
	d_free(&x);

	ck_assert_int_eq(true, _test_free_sigill_raised);

	detach_signal_handlers(SIGILL);

	// Free the memory that we did actually allocate.
	d_free(p);
}
END_TEST

START_TEST(test_free_sbrk_failure) {
	// Allocate something.
	void *ptr = d_malloc(8);

	// Now configure sbrk() to call our custom_sbrk function. This will proxy
	// the request through to sbrk() (in order to preserve the integrity of the
	// heap and actually free this memory), and return (void *)-1, indicating
	// failure.
	attach_sbrk_handler(custom_sbrk);

	// Attempt to free an invalid pointer after allocating something. This
	// should result in a crash.
	attach_signal_handler(SIGILL, _test_free_sigill_handler);

	ck_assert_int_eq(false, _test_free_sigill_raised);

	// Attempt to free our allocated memory. This will fail, but the memory will
	// actually be freed successfully.
	d_free(ptr);

	ck_assert_int_eq(true, _test_free_sigill_raised);

	detach_signal_handlers(SIGILL);

	// This failure should not cause a crash in client programs/libraries. A
	// crash in this test should be considered a failure.
	remove_sbrk_handlers();
}
END_TEST

START_TEST(test_free_unused_chunk) {
	void *ptr0 = d_malloc(4);
	void *ptr1 = d_malloc(4);
	// ptr0 will not be released back to the OS, because there is an in-use
	// chunk between it and the program break.
	d_free(ptr0);

	attach_signal_handler(SIGILL, _test_free_sigill_handler);
	ck_assert_int_eq(false, _test_free_sigill_raised);
	d_free(ptr0);
	detach_signal_handlers(SIGILL);
	ck_assert_int_eq(true, _test_free_sigill_raised);

	// Now release the rest of the memory.
	d_free(ptr1);
}
END_TEST

Suite *d_free_test_suite() {
	// Freed in srunner_free().
    TCase* test_case = tcase_create("d_free test case");
    tcase_add_checked_fixture(test_case, free_tests_setup, free_tests_teardown);

	tcase_add_test(test_case, test_free_noalloc);
	tcase_add_test(test_case, test_free_invalid);
    tcase_add_loop_test(test_case, ensure_single_chunk_is_released, 1, 32);
	tcase_add_test(test_case, test_greedy_free);
	tcase_add_test(test_case, test_free_sbrk_failure);
	tcase_add_test(test_case, test_free_unused_chunk);

	// Freed in srunner_free().
    Suite *suite = suite_create("free tests");
    suite_add_tcase(suite, test_case);
    return suite;
}
