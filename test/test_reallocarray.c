#include <check.h>
#include <stdbool.h>

#include "dalloc.h"
#include "dalloc_io.h"
#include "test_reallocarray.h"
#include "test_util.h"

static bool sigill_raised;

void reallocarray_tests_setup() {
	set_log_level(DALLOC_LOG_LEVEL_NONE);
	sigill_raised = false;
}

void reallocarray_tests_teardown() {
	
}

START_TEST(test_reallocarray) {
	const size_t initial_size = 256;
	const size_t nelem = 256;
	const size_t elem_size = sizeof(uint32_t);

	void *ptr0 = d_malloc(initial_size);
	void *ptr1 = d_malloc(initial_size);
	ck_assert_ptr_nonnull(ptr0);
	ck_assert_ptr_nonnull(ptr1);

	fill_memory(initial_size, ptr0);
	fill_memory(initial_size, ptr1);

	void *ptr2 = d_reallocarray(ptr0, nelem, elem_size);
	ck_assert_ptr_nonnull(ptr2);

	assert_ptr_contents_equal(initial_size, ptr1, ptr2);

	d_free(ptr2);
	d_free(ptr1);
}
END_TEST

START_TEST(test_reallocarray_integer_overflow) {
	// Attempt to reallocarray with nmemb high enough to cause integer overflow.
	size_t nelem = SIZE_MAX;
	size_t elem_size = sizeof(int32_t);

	void *ptr0 = d_malloc(16);
	ck_assert_ptr_nonnull(ptr0);

	// This nmemb * elem_size is > SIZE_MAX and will overflow. reallocarray()
	// should return NULL here.
	void *ptr1 = d_reallocarray(ptr0, nelem, elem_size);
	ck_assert_ptr_null(ptr1);

	d_free(ptr0);
}
END_TEST

Suite *d_reallocarray_test_suite() {
    TCase* test_case = tcase_create("reallocarray Test Case");
    tcase_add_checked_fixture(test_case, reallocarray_tests_setup, reallocarray_tests_teardown);

	tcase_add_test(test_case, test_reallocarray);
	tcase_add_test(test_case, test_reallocarray_integer_overflow);

	Suite* suite = suite_create("reallocarray Tests");
    suite_add_tcase(suite, test_case);
    return suite;
}
