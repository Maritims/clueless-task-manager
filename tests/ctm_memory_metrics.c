#include "ctm_memory_metrics.h"
#include "test.h"
#include "../src/ctm_memory_metrics_internal.h"

TEST(read) {
    /* arrange */
    int                         success;
    ctm_memory_metrics_status_t actual;
    ctm_memory_metrics_t        memory_metrics;

    /* act */
    actual = ctm_memory_metrics_read(&memory_metrics);

    /* assert */
    success = assert_int_equality(CTM_MEMORY_METRICS_SUCCESS, actual, "ctm_memory_metrics_read() should not return CTM_MEMORY_METRICS_SUCCESS");

    return success;
}

#define TESTS \
    TEST_CASE(read)

RUN_TEST_SUITE(TESTS)
