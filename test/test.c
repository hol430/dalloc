#include <stdio.h>
#include <assert.h>
#include <check.h>
#include <stdlib.h>
#include <stdint.h>

#include "test_free.h"
#include "test_heap_manip.h"
#include "test_heap_traversal.h"
#include "test_malloc.h"

Suite** build_test_suite(int* num_suites) {
    *num_suites = 4;
    Suite** test_suites = (Suite**)malloc(*num_suites * sizeof(Suite*));
    test_suites[0] = d_malloc_test_suite();
    test_suites[1] = d_free_test_suite();
    test_suites[2] = d_heap_manip_test_suite();
    test_suites[3] = d_heap_traversal_test_suite();

    return test_suites;
}

int main(int argc, char** argv) {
    int num_suites;
    Suite** all_suites = build_test_suite(&num_suites);
    int num_failures = 0;
    for (uint32_t i = 0; i < num_suites; i++) {
        Suite* suite = all_suites[i];
        SRunner* runner = srunner_create(suite);
        srunner_run_all(runner, CK_NORMAL);
        num_failures += srunner_ntests_failed(runner);
        srunner_free(runner);
    }
    free(all_suites);

    return num_failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
