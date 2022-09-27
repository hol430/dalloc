#include <stdio.h>
#include <assert.h>
#include <check.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "dalloc_io.h"
#include "dalloc_internal.h"

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

    // Attempt to write to this memory.
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

Suite *d_malloc_test_suite() {
	Suite* suite;
    TCase* test_case;

    suite = suite_create("malloc Tests");
    test_case = tcase_create("malloc Test Case");

    tcase_add_loop_test(test_case, allocate, 1, 8);
    tcase_add_loop_test(test_case, allocate_2n, 4, 16);
    tcase_add_test(test_case, allocate_zero);
    tcase_add_checked_fixture(test_case, malloc_tests_setup, malloc_tests_teardown);

    suite_add_tcase(suite, test_case);
    return suite;
}
