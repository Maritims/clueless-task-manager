#include "mem.h"
#include "mem_internal.h"
#include "test.h"

TEST(read)
{
    /* arrange */
    int          success;
    mem_result_t actual;
    mem_t        memory_metrics;

    /* act */
    actual = mem_read(&memory_metrics);

    /* assert */
    success = assert_int_equality(MEM_SUCCESS, actual, mem_strerror(actual));

    return success;
}

#define TESTS \
    TEST_CASE(read)

RUN_TEST_SUITE(TESTS)
