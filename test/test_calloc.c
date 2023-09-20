#include <check.h>

#include "dalloc.h"
#include "dalloc_io.h"
#include "test_calloc.h"
#include "test_util.h"

void calloc_tests_setup() {
	set_log_level(DALLOC_LOG_LEVEL_NONE);
}

void calloc_tests_teardown() {

}

START_TEST(callocate) {
	size_t n = 1 << _i;
	double *ptr = (double *)d_calloc(n, sizeof(double));
	ck_assert_ptr_nonnull(ptr);
	// Ensure the memory is writable. Actually, this doesn't guarantee that
	// it's writable; if we're lucky this will fail on an invalid pointer,
	// but even if unlucky, valgrind should tell us if these writes are
	// illegal.
	for (size_t i = 0; i < n; i++) {
		ptr[i] = __DBL_MAX__;
	}
	d_free(ptr);
}
END_TEST

START_TEST(callocate_too_large) {
	// Get a number of elements which is large enough to cause an integer
	// overflow. Ensure that NULL is returned in this case.
	void *ptr = d_calloc(SIZE_MAX, sizeof(int32_t));
	ck_assert_ptr_null(ptr);
}
END_TEST

void *sbrk_ret_minus1(intptr_t ptr) {
	return (void *)-1;
}

START_TEST(test_malloc_failure) {
	attach_sbrk_handler(sbrk_ret_minus1);
	void *ptr = d_calloc(2, 8);
	remove_sbrk_handlers();
	ck_assert_ptr_null(ptr);
}
END_TEST

Suite *d_calloc_test_suite() {
    TCase* test_case = tcase_create("calloc test case");
    tcase_add_checked_fixture(test_case, calloc_tests_setup, calloc_tests_teardown);

    tcase_add_loop_test(test_case, callocate, 0, 10);
	tcase_add_test(test_case, callocate_too_large);
	tcase_add_test(test_case, test_malloc_failure);

	Suite* suite = suite_create("calloc tests");
    suite_add_tcase(suite, test_case);
    return suite;
}
