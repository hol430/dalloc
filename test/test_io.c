#include <check.h>
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "dalloc_io.h"
#include "test_io.h"

#define OUT_LOG_FILE_NAME "dalloc_test_io.log"
#define ERR_LOG_FILE_NAME "dalloc_test_io.err"

FILE *stdout_log;
FILE *stderr_log;

void io_tests_setup() {
	stdout_log = freopen(OUT_LOG_FILE_NAME, "w", stdout);
	stderr_log = freopen(ERR_LOG_FILE_NAME, "w", stderr);

	// Default logging behaviour for these tests is to log everything.
	// Any test which relies on different behaviour will need to
	// manually configure the log level.
	set_log_level(DALLOC_LOG_LEVEL_DEBUG);
}

void io_tests_teardown() {
	uint32_t num_results = 4;
	uint32_t results[num_results];

	results[0] = fclose(stdout_log);
	stdout_log = NULL;
	results[1] = remove(OUT_LOG_FILE_NAME);

	results[2] = fclose(stderr_log);
	stderr_log = NULL;
	results[3] = remove(ERR_LOG_FILE_NAME);

	for (uint32_t i = 0; i < num_results; i++) {
		ck_assert_int_eq(0, results[i]);
	}
}

/*
Read entire contents of file into a null-terminated string. File must
not be open when this is called. The string is owned by the caller.
*/
char *read_to_end(const char *file_name) {
	FILE *file = fopen(file_name, "r");

	// Get file length.
	fseek(file, 0, SEEK_END);
	int32_t file_length = ftell(file);

	// Allocate a buffer to hold file contents. This would be
	// problematic for large files.
	char *buf = malloc(file_length + 1);
	buf[file_length] = 0;

	// Move back to start of file.
	fseek(file, 0, SEEK_SET);

	// Read contents of file. todo: read in chunks?
	int32_t res = fread(buf, 1, file_length, file);
	ck_assert_int_eq(file_length, res);

	fclose(file);

	return buf;
}

/*
Return null-terminated stdout generated by the test up to this point.
The result is owned, and must be freed by, the caller.
*/
char *get_test_stdout() {
	fclose(stdout_log);
	char *result = read_to_end(OUT_LOG_FILE_NAME);
	stdout_log = freopen(OUT_LOG_FILE_NAME, "w", stdout);
	return result;
}

/*
Return null-terminated stderr generated by the test up to this point.
The result is owned, and must be freed by, the caller.
*/
char *get_test_stderr() {
	fclose(stderr_log);
	char *result = read_to_end(ERR_LOG_FILE_NAME);
	stderr_log = freopen(ERR_LOG_FILE_NAME, "w", stderr);
	return result;
}

void assert_regex_match(const char *str, const char *pattern) {
	regex_t regex;

	// Compile the regular expression.
	int32_t res = regcomp(&regex, pattern, 0);
	ck_assert_int_eq(0, res);

	// Execute the regular expression.
	res = regexec(&regex, str, 0, NULL, 0);
	ck_assert_int_eq(0, res);
}

START_TEST(test_write_error) {
	char *msg = "short error message";

	log_error(msg);

	char *output = get_test_stdout();
	char *error = get_test_stderr();

	ck_assert_int_eq(0, strlen(output));

	char *fmt = "dalloc .* ERROR: %s";
	char pattern[strlen(fmt) + strlen(msg) - 1];
	sprintf(pattern, fmt, msg);
	assert_regex_match(error, pattern);

	free(output);
	free(error);
}
END_TEST

Suite *d_io_test_suite() {
	Suite* suite;
    TCase* test_case;

    suite = suite_create("utils tests");
    test_case = tcase_create("d_utils test case");
	tcase_add_checked_fixture(test_case, io_tests_setup, io_tests_teardown);
    suite_add_tcase(suite, test_case);

	tcase_add_test(test_case, test_write_error);

	return suite;
}
