#include <check.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "dalloc.h"
#include "dalloc_io.h"
#include "test_realloc.h"
#include "test_util.h"

static bool sigill_raised;

void realloc_tests_setup() {
	set_log_level(DALLOC_LOG_LEVEL_NONE);
	sigill_raised = false;
}

void realloc_tests_teardown() {
	
}

void _realloc_sigill_handler(int32_t signum) {
	ck_assert_int_eq(SIGILL, signum);
	sigill_raised = true;
}

START_TEST(test_realloc_null) {
	size_t size = 1 << _i;
	void *ptr = d_realloc(NULL, size);
	ck_assert_ptr_nonnull(ptr);
	// Try writing to this memory. This won't necessarily cause the test to fail
	// if ptr points to invalid memory, but it will trigger memcheck warnings.
	fill_memory(size, ptr);
	d_free(ptr);
}
END_TEST

START_TEST(test_realloc_size0) {
	void *ptr = d_malloc(4);
	ck_assert_ptr_nonnull(ptr);
	ptr = d_realloc(ptr, 0);
	ck_assert_ptr_null(ptr);
	// todo: verify that memory was released?
}
END_TEST

START_TEST(test_realloc_without_allocating) {
	// Test the effect of realloc'ing an invalid pointer without first alloc'ing
	// anything (ie heap is uninitialised).
	void *ptr = malloc(8);

	ck_assert_int_eq(false, sigill_raised);
	attach_signal_handler(SIGILL, _realloc_sigill_handler);
	void *d_ptr = d_realloc(ptr, 4);
	detach_signal_handlers(SIGILL);
	free(ptr);
	ck_assert_int_eq(true, sigill_raised);
	ck_assert_ptr_null(d_ptr);
}
END_TEST

START_TEST(test_realloc_invalid_ptr) {
	// Initialise the heap by allocating something and then test the effect of
	// realloc-ing an invalid pointer.

	// Initialise the dalloc heap.
	void *ptr0 = d_malloc(8);

	// Add a SIGILL handler to trap the signal that should be raised.
	ck_assert_int_eq(false, sigill_raised);
	attach_signal_handler(SIGILL, _realloc_sigill_handler);

	int x;
	void *d_ptr = d_realloc(&x, 4);

	// Cleanup before making any assertions.
	detach_signal_handlers(SIGILL);
	d_free(ptr0);

	ck_assert_int_eq(true, sigill_raised);
	ck_assert_ptr_null(d_ptr);
}
END_TEST

START_TEST(test_realloc_unused_ptr) {
	void *ptr0 = d_malloc(8);
	ck_assert_ptr_nonnull(ptr0);
	void *ptr1 = d_malloc(8);
	ck_assert_ptr_nonnull(ptr1);
	d_free(ptr0);

	// ptr0 could not have been released to the OS, because ptr1 is lies between
	// it and the program break.
	ck_assert_int_eq(false, sigill_raised);
	attach_signal_handler(SIGILL, _realloc_sigill_handler);

	// Here we are trying to reuse freed memory, so we should see SIGILL.
	void *ptr2 = d_realloc(ptr0, 16);

	// Cleanup before making any assertions.
	detach_signal_handlers(SIGILL);
	d_free(ptr1);

	ck_assert_int_eq(true, sigill_raised);
	ck_assert_ptr_null(ptr2);
}
END_TEST

START_TEST(test_realloc_same_size) {
	// Test reallocing to the same size. This should return the same pointer.
	size_t size = 1 << _i;
	void *ptr0 = d_malloc(size);

	// Populate the allocated memory.
	fill_memory(size, ptr0);

	void *ptr1 = d_realloc(ptr0, size);

	// Ensure that the same pointer was returned, and that its contents weren't
	// modified.
	ck_assert_ptr_eq(ptr0, ptr1);
	assert_ptr_contents_equal(size, ptr0, ptr1);

	// ptr1 == ptr0, so no need to free both
	d_free(ptr0);
}
END_TEST

START_TEST(test_realloc_smaller) {
	// Test reallocing to a smaller size, with enough leftover that a new chunk
	// may be created.

	// alloc size is 1024, realloc size is 512. This leaves plenty of room for
	// the remainder to be moved into a new chunk.
	const size_t n = 10;
	size_t size = 1 << n;
	size_t new_size = 1 << (n - 1);

	// Perform (re/)allocation
	void *ptr0 = d_malloc(size);
	fill_memory(size, ptr0);

	void *ptr1 = d_realloc(ptr0, new_size);

	// First, ensure that the returned pointer is the same as the original.
	ck_assert_ptr_eq(ptr0, ptr1);
	assert_ptr_contents_equal(new_size, ptr0, ptr1);

	d_free(ptr0);
}
END_TEST

START_TEST(test_realloc_slightly_smaller) {
	// Test reallocing to a smaller size, with not enough leftover to create a
	// new chunk.

	// alloc size is 1024, realloc size is 512. This leaves plenty of room for
	// the remainder to be moved into a new chunk.
	const size_t size = 16;
	const size_t new_size = 8;

	// Perform (re/)allocation
	void *ptr0 = d_malloc(size);
	fill_memory(size, ptr0);
	void *ptr1 = d_realloc(ptr0, new_size);

	// First, ensure that the returned pointer is the same as the original.
	ck_assert_ptr_eq(ptr0, ptr1);
	assert_ptr_contents_equal(new_size, ptr0, ptr1);

	// Now we allocate a new chunk and verify that it's allocated past the
	// current brk.
	void *start = sbrk(0);
	void *ptr2 = d_malloc(2);
	ck_assert_msg(ptr2 > start, "Invalid memory allocation");

	// ptr0 == ptr1
	d_free(ptr0);
	d_free(ptr2);
}
END_TEST

START_TEST(test_realloc_larger) {
	// realloc a chunk of memory to a larger size.
	const size_t size = 512;
	const size_t new_size = 1024;

	// Perform the initial allocation.
	void *ptr0 = d_malloc(size);
	void *ptr1 = d_malloc(size);
	ck_assert_ptr_nonnull(ptr0);

	// Populate the new region of memory.
	fill_memory(size, ptr0);
	fill_memory(size, ptr1);

	// Now reallocate to a larger size.
	void *ptr2 = d_realloc(ptr0, new_size);
	ck_assert_ptr_nonnull(ptr2);

	// Ensure that memory has been copied if necessary.
	assert_ptr_contents_equal(size, ptr1, ptr2);

	// Write to the new region of memory.
	fill_memory(new_size, ptr2);

	// Release memory.
	d_free(ptr1);
	d_free(ptr2);
}
END_TEST

Suite *d_realloc_test_suite() {
    TCase* test_case = tcase_create("realloc Test Case");
    tcase_add_checked_fixture(test_case, realloc_tests_setup, realloc_tests_teardown);

	tcase_add_loop_test(test_case, test_realloc_null, 0, 10);
	tcase_add_loop_test(test_case, test_realloc_same_size, 0, 10);
	tcase_add_test(test_case, test_realloc_size0);
	tcase_add_test(test_case, test_realloc_without_allocating);
	tcase_add_test(test_case, test_realloc_invalid_ptr);
	tcase_add_test(test_case, test_realloc_unused_ptr);
	tcase_add_test(test_case, test_realloc_smaller);
	tcase_add_test(test_case, test_realloc_slightly_smaller);
	tcase_add_test(test_case, test_realloc_larger);

	Suite* suite = suite_create("realloc Tests");
    suite_add_tcase(suite, test_case);
    return suite;
}
