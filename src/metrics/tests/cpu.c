#include <unistd.h>

#include "metrics/cpu.h"
#include "test.h"

TEST(get) {
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

TEST(get_total_usage) {
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

#define TESTS \
    TEST_CASE(get)\
    TEST_CASE(get_total_usage)

RUN_TEST_SUITE(TESTS)