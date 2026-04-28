#include "process.h"
#include "process_internal.h"
#include "test.h"

TEST(read)
{
    /* arrange */
    int              success;
    process_result_t actual;
    process_t        process;

    /* act */
    actual = process_read(1, &process);

    /* assert */
    success = assert_int_equality(PROCESS_SUCCESS_READ, actual, process_strerror(actual));

    return success;
}

#define TESTS \
    TEST_CASE(read)

RUN_TEST_SUITE(TESTS)
