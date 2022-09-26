#include <stdio.h>
#include <assert.h>
#include <check.h>
#include <stdlib.h>
#include <stdint.h>

#include "dalloc_internal.h"

void setup() {

}

void teardown() {
	
}

void test_allocate(size_t size) {
    unsigned char* ptr = d_malloc(size);

    ck_assert_ptr_nonnull(ptr);

    // Attempt to write to this memory.
    for (size_t i = 0; i < size; i++) {
        ptr[i] = 0;
    }

    d_free(ptr);
}

START_TEST(simple) {
    for (size_t i = 0; i < 8; i++) {
        test_allocate(i);
    }
    for (int i = 0; i < 10; i++) {
        test_allocate(2 << i);
    }
}

Suite *malloc_test_suite() {
	Suite* suite;
    TCase* test_case;

    suite = suite_create("malloc Tests");
    test_case = tcase_create("malloc Test Case");

    tcase_add_test(test_case, simple);
    tcase_add_checked_fixture(test_case, setup, teardown);

    suite_add_tcase(suite, test_case);
    return suite;
}