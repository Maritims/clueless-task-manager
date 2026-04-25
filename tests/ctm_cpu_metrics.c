#include <unistd.h>

#include "ctm_cpu_metrics.h"

#include <limits.h>

#include "../src/internal/ctm_cpu_metrics_internal.h"
#include "test.h"

TEST(read) {
    /* arrange */
    int                      success;
    ctm_cpu_metrics_status_t actual;
    ctm_cpu_metrics_t        cpu_metrics;

    /* act */
    actual = ctm_cpu_metrics_read(&cpu_metrics);

    /* assert */
    success = assert_int_equality(CTM_CPU_METRICS_SUCCESS, actual, "ctm_cpu_Metrics_read() should return CTM_CPU_METRICS_SUCCESS");

    return success;
}

TEST(get_total_usage) {
    /* arrange */
    int                      success;
    ctm_cpu_metrics_t        current, previous;
    unsigned long            out_value = ULONG_MAX;
    ctm_cpu_metrics_status_t actual;

    ctm_cpu_metrics_read(&previous);
    sleep(1);
    ctm_cpu_metrics_read(&current);

    /* act */
    actual = ctm_cpu_metrics_get_total_usage(&current, &previous, &out_value);

    /* assert */
    success = assert_int_equality(CTM_CPU_METRICS_SUCCESS, actual, "ctm_cpu_metrics_get_total_usage() should return CTM_CPU_METRICS_SUCCESS");

    return success;
}

#define TESTS \
    TEST_CASE(read)\
    TEST_CASE(get_total_usage)

RUN_TEST_SUITE(TESTS)
