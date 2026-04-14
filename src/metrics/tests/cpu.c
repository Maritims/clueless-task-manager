#include <unistd.h>

#include "metrics/cpu.h"
#include "test.h"

int test_get(void) {
    /* arrange */
    int  success;
    CPU* actual;

    /* act */
    actual = cpu_get();
    cpu_free(actual);

    /* assert */
    success = assert_not_null(actual, "cpu_get() should not return NULL");

    return success;
}

int test_get_total_usage(void) {
    /* arrange */
    int  success;
    CPU *current, *previous;
    long actual;

    previous = cpu_get();
    sleep(1);
    current = cpu_get();

    /* act */
    actual = cpu_get_total_usage(current, previous);

    /* assert */
    success = assert_long_greater_than(0, actual, "cpu_get_total_usage() should return a positive value");

    return success;
}

int main(void) {
    const TestCase test_cases[] = {
        {"cpu_get", test_get},
        {"get_total_usage", test_get_total_usage}
    };
    const size_t test_count = sizeof(test_cases) / sizeof(test_cases[0]);
    return run_all_tests(test_cases, test_count);
}
