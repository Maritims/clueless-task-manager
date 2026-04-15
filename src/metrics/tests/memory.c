#include "metrics/memory.h"
#include "test.h"

TEST(get) {
    /* arrange */
    int     success;
    Memory* actual;

    /* act */
    actual = memory_get();

    /* assert */
    success = assert_not_null(actual, "memory_get() should not return NULL");
    if (success == 0) success = assert_unsigned_long_equality(0, memory_get_total(actual), "memory_get() should return 0");
    if (success == 0) success = assert_unsigned_long_equality(0, memory_get_free(actual), "memory_get_free() should return 0");
    if (success == 0) success = assert_unsigned_long_equality(0, memory_get_available(actual), "memory_get_available() should return 0");
    memory_free(actual);

    return success;
}

#define TESTS \
    TEST_CASE(get)

RUN_TEST_SUITE(TESTS)