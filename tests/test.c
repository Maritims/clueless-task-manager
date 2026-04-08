#include "test.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int assert_null(const void* value, const char* message) {
    if (value == NULL) {
        return 0;
    }

    fprintf(stderr, "Assertion failed! %s\n", message);
    return 1;
}

int assert_not_null(const void* value, const char* message) {
    if (value != NULL) {
        return 0;
    }

    fprintf(stderr, "Assertion failed! %s\n", message);
    return 1;
}

int assert_int_equality(const int expected, const int actual, const char* message) {
    assert(message != NULL && strlen(message) > 0 && "message must not be NULL or empty");

    if (expected == actual) {
        return 0;
    }

    fprintf(stderr, "Assertion failed! Expected %d, but got %d: %s\n", expected, actual, message);
    return 1;
}

int assert_long_greater_than(const long expected, const long actual, const char* message) {
    assert(message != NULL && strlen(message) > 0 && "message must not be NULL or empty");

    if (actual > expected) {
        return 0;
    }

    fprintf(stderr, "Assertion failed! Expected %ld to be greater than %ld: %s\n", actual, expected, message);
    return 1;
}

int assert_unsigned_int_equality(const unsigned int expected, const unsigned int actual, const char* message) {
    assert(message != NULL && strlen(message) > 0 && "message must not be NULL or empty");

    if (expected == actual) {
        return 0;
    }

    fprintf(stderr, "Assertion failed! Expected %d, but got %d: %s\n", expected, actual, message);
    return 1;
}

int assert_unsigned_long_equality(const unsigned long expected, const unsigned long actual, const char* message) {
    assert(message != NULL && strlen(message) > 0 && "message must not be NULL or empty");

    if (expected == actual) {
        return 0;
    }

    fprintf(stderr, "Assertion failed! Expected %lu, but got %lu: %s\n", expected, actual, message);
    return 1;
}

int run_all_tests(const TestCase* test_cases, const size_t test_count) {
    int      failed_tests = 0;
    size_t   i;
    TestCase test_case;
    char     test_output_buffer[1024];

    assert(test_cases != NULL && "test_cases must not be NULL");
    assert(test_count > 0 && "test_count must be greater than 0");

    printf("Running %lu test(s)...\n", test_count);

    for (i = 0; i < test_count; i++) {
        test_case = test_cases[i];

        snprintf(test_output_buffer, sizeof(test_output_buffer), "  [%lu/%lu]: %s...", i + 1, test_count, test_case.name);

        if (test_case.func() == 0) {
            snprintf(test_output_buffer + strlen(test_output_buffer), sizeof(test_output_buffer) - strlen(test_output_buffer), "passed\n");
        } else {
            snprintf(test_output_buffer + strlen(test_output_buffer), sizeof(test_output_buffer) - strlen(test_output_buffer), "failed\n");
            failed_tests++;
        }

        printf("%s", test_output_buffer);
    }

    if (failed_tests == 0) {
        printf("All tests passed\n");
    } else {
        printf("%d tests failed\n", failed_tests);
    }

    return failed_tests;
}
