#include <check.h>
#include <stdbool.h>
#include <stdlib.h>

#include "chunk.h"
#include "test_heap_manip.h"

chunk_t chunk_0;
chunk_t chunk_1;
chunk_t chunk_2;
chunk_t chunk_3;

void heap_manip_test_setup() {
	chunk_0.size = 0;
	chunk_0.in_use = false;
	chunk_0.start = (void *)1;

	chunk_1.size = 1;
	chunk_1.in_use = true;
	chunk_1.start = (void *)2;

	chunk_2.size = 2;
	chunk_2.in_use = false;
	chunk_2.start = (void *)4;

	chunk_3.size = 3;
	chunk_3.in_use = true;
	chunk_3.start = (void *)8;

	chunk_0.iter = &chunk_1;
	chunk_1.iter = (void *)( (uintptr_t)(&chunk_0) ^ (uintptr_t)(&chunk_2) );
	chunk_2.iter = (void *)( (uintptr_t)(&chunk_1) ^ (uintptr_t)(&chunk_3) );
	chunk_3.iter = &chunk_2;
}

void heap_manip_test_teardown() {

}

START_TEST(test_next_from_start) {
	chunk_t *nxt = next(&chunk_0, NULL);
	// Next after chunk_0 should be chunk_1.
	ck_assert_ptr_eq(&chunk_1, nxt);
}
END_TEST

START_TEST(test_next_from_mid) {
	chunk_t *nxt = next(&chunk_1, &chunk_0);
	// Next after chunk_1 should be chunk_2.
	ck_assert_ptr_eq(&chunk_2, nxt);
}
END_TEST

START_TEST(test_next_from_end) {
	chunk_t *nxt = next(&chunk_3, &chunk_2);
	ck_assert_ptr_null(nxt);
}
END_TEST

START_TEST(test_prev_from_start) {
	chunk_t *prv = prev(&chunk_0, &chunk_1);
	ck_assert_ptr_null(prv);
}
END_TEST

START_TEST(test_prev_from_mid) {
	chunk_t *prv = prev(&chunk_1, &chunk_2);
	// Prev before chunk_1 should be chunk_0.
	ck_assert_ptr_eq(&chunk_0, prv);
}
END_TEST

START_TEST(test_prev_from_end) {
	chunk_t *prv = prev(&chunk_3, NULL);
	// Prev before chunk_3 should be chunk_2.
	ck_assert_ptr_eq(&chunk_2, prv);
}
END_TEST

START_TEST(test_append_at_start) {
	// todo: this may be redundant with append_in_middle
	chunk_t new;
	new.size = 3;
	append(NULL, &chunk_0, &new);

	// chunk_0 -> new -> chunk_1 -> ...
	ck_assert_ptr_eq(&new, next(&chunk_0, NULL));
	ck_assert_ptr_eq(&chunk_1, next(&new, &chunk_0));

	ck_assert_ptr_eq(&chunk_0, prev(&new, &chunk_1));
	ck_assert_ptr_eq(&new, prev(&chunk_1, &chunk_2));
}
END_TEST

START_TEST(test_append_in_middle) {
	chunk_t new;
	append(&chunk_0, &chunk_1, &new);

	// ... -> chunk_1 -> new -> chunk_2 -> ...
	ck_assert_ptr_eq(&new, next(&chunk_1, &chunk_0));
	ck_assert_ptr_eq(&chunk_2, next(&new, &chunk_1));

	ck_assert_ptr_eq(&new, prev(&chunk_2, &chunk_3));
	ck_assert_ptr_eq(&chunk_1, prev(&new, &chunk_2));
}
END_TEST

START_TEST(test_append_to_end) {
	chunk_t new;
	append(&chunk_2, &chunk_3, &new);

	// ... -> chunk_2 -> chunk_3 -> new
	ck_assert_ptr_eq(&chunk_3, next(&chunk_2, &chunk_1));
	ck_assert_ptr_eq(&new, next(&chunk_3, &chunk_2));

	ck_assert_ptr_eq(&chunk_3, prev(&new, NULL));
	ck_assert_ptr_eq(&chunk_2, prev(&chunk_3, &new));
}
END_TEST

START_TEST(test_remove_before_at_start) {
	remove_before(NULL, &chunk_0);

	// chunk_1 -> chunk_2 -> ...
	ck_assert_ptr_eq(&chunk_2, next(&chunk_1, NULL));
	ck_assert_ptr_eq(&chunk_3, next(&chunk_2, &chunk_1));

	ck_assert_ptr_eq(&chunk_2, prev(&chunk_3, NULL));
	ck_assert_ptr_eq(&chunk_1, prev(&chunk_2, &chunk_3));
}
END_TEST

START_TEST(test_remove_before_in_middle) {
	remove_before(&chunk_2, &chunk_1);

	// chunk_0 -> chunk_2 -> ...
	ck_assert_ptr_eq(&chunk_2, next(&chunk_0, NULL));
	ck_assert_ptr_eq(&chunk_3, next(&chunk_2, &chunk_0));

	ck_assert_ptr_eq(&chunk_2, prev(&chunk_3, NULL));
	ck_assert_ptr_eq(&chunk_0, prev(&chunk_2, &chunk_3));
}
END_TEST

START_TEST(test_remove_before_at_end) {
	remove_before(NULL, &chunk_3);

	// ... -> chunk_1 -> chunk_2
	ck_assert_ptr_eq(&chunk_1, next(&chunk_0, NULL));
	ck_assert_ptr_eq(&chunk_2, next(&chunk_1, &chunk_0));

	ck_assert_ptr_eq(&chunk_1, prev(&chunk_2, NULL));
	ck_assert_ptr_eq(&chunk_0, prev(&chunk_1, &chunk_2));
}
END_TEST

START_TEST(test_remove_after_at_start) {
	remove_after(NULL, &chunk_0);

	// chunk_1 -> chunk_2 -> ...
	ck_assert_ptr_eq(&chunk_2, next(&chunk_1, NULL));
	ck_assert_ptr_eq(&chunk_3, next(&chunk_2, &chunk_1));

	ck_assert_ptr_eq(&chunk_2, prev(&chunk_3, NULL));
	ck_assert_ptr_eq(&chunk_1, prev(&chunk_2, &chunk_3));
}
END_TEST

START_TEST(test_remove_after_in_middle) {
	remove_after(&chunk_1, &chunk_2);

	// ... -> chunk_1 -> chunk_3
	ck_assert_ptr_eq(&chunk_1, next(&chunk_0, NULL));
	ck_assert_ptr_eq(&chunk_3, next(&chunk_1, &chunk_0));

	ck_assert_ptr_eq(&chunk_1, prev(&chunk_3, NULL));
	ck_assert_ptr_eq(&chunk_0, prev(&chunk_1, &chunk_3));
}
END_TEST

START_TEST(test_remove_after_at_end) {
	remove_after(&chunk_2, &chunk_3);

	// ... -> chunk_1 -> chunk_2
	ck_assert_ptr_eq(&chunk_1, next(&chunk_0, NULL));
	ck_assert_ptr_eq(&chunk_2, next(&chunk_1, &chunk_0));

	ck_assert_ptr_eq(&chunk_1, prev(&chunk_2, NULL));
	ck_assert_ptr_eq(&chunk_0, prev(&chunk_1, &chunk_2));
}
END_TEST

Suite *d_heap_manip_test_suite() {
	Suite* suite;
    TCase* test_case;

    suite = suite_create("heap manipulation tests");
    test_case = tcase_create("heap manip test case");

    tcase_add_checked_fixture(test_case, heap_manip_test_setup, heap_manip_test_teardown);

	tcase_add_test(test_case, test_next_from_start);
	tcase_add_test(test_case, test_next_from_mid);
	tcase_add_test(test_case, test_next_from_end);
	tcase_add_test(test_case, test_prev_from_start);
	tcase_add_test(test_case, test_prev_from_mid);
	tcase_add_test(test_case, test_prev_from_end);
	tcase_add_test(test_case, test_append_at_start);
	tcase_add_test(test_case, test_append_in_middle);
	tcase_add_test(test_case, test_append_to_end);
	tcase_add_test(test_case, test_remove_before_at_start);
	tcase_add_test(test_case, test_remove_before_in_middle);
	tcase_add_test(test_case, test_remove_before_at_end);
	tcase_add_test(test_case, test_remove_after_at_start);
	tcase_add_test(test_case, test_remove_after_in_middle);
	tcase_add_test(test_case, test_remove_after_at_end);

    suite_add_tcase(suite, test_case);
    return suite;
}
