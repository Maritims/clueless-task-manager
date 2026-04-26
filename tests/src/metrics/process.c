#include "metrics/ctm_process_metrics.h"
#include "../../include/test.h"
#include "internal/ctm_process_metrics_internal.h"

TEST(read) {
    /* arrange */
    int                          success;
    ctm_process_metrics_status_t actual;
    ctm_process_metrics_t        process_metrics;

    /* act */
    actual = ctm_process_metrics_read(1, &process_metrics);

    /* assert */
    success = assert_int_equality(CTM_PROCESS_METRICS_SUCCESS, actual, ctm_process_metrics_strerror(actual));

    return success;
}

#define TESTS \
    TEST_CASE(read)

RUN_TEST_SUITE(TESTS)
