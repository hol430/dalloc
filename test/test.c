#include <stdio.h>
#include <assert.h>
#include <check.h>
#include <stdlib.h>
#include <stdint.h>

#include "test_free.h"
#include "test_heap_manip.h"
#include "test_heap_traversal.h"
#include "test_io.h"
#include "test_calloc.h"
#include "test_malloc.h"
#include "test_realloc.h"
#include "test_reallocarray.h"
#include "test_utils.h"

Suite **build_test_suite(size_t *num_suites) {
    *num_suites = 9;
    Suite **test_suites = (Suite **)malloc(*num_suites * sizeof(Suite *));
    test_suites[0] = d_calloc_test_suite();
    test_suites[1] = d_malloc_test_suite();
    test_suites[2] = d_realloc_test_suite();
    test_suites[3] = d_reallocarray_test_suite();
    test_suites[4] = d_free_test_suite();
    test_suites[5] = d_heap_manip_test_suite();
    test_suites[6] = d_heap_traversal_test_suite();
    test_suites[7] = d_utils_test_suite();
    test_suites[8] = d_io_test_suite();

    return test_suites;
}

int main(int argc, char **argv) {
    size_t num_suites;
    Suite **all_suites = build_test_suite(&num_suites);
    int num_failures = 0;
    for (size_t i = 0; i < num_suites; i++) {
        Suite *suite = all_suites[i];
        SRunner *runner = srunner_create(suite);
        srunner_run_all(runner, CK_NORMAL);
        num_failures += srunner_ntests_failed(runner);
        srunner_free(runner);
    }
    free(all_suites);

    return num_failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
