#include "cpu.h"
#include <limits.h>
#include <unistd.h>
#include "test.h"
#include "cpu_internal.h"

TEST(read)
{
    /* arrange */
    int          success;
    cpu_result_t actual;
    cpu_t        cpu_metrics;

    /* act */
    actual = cpu_read(&cpu_metrics);

    /* assert */
    success = assert_int_equality(CPU_SUCCESS, actual, "ctm_cpu_Metrics_read() should return CTM_CPU_METRICS_SUCCESS");

    return success;
}

TEST(get_total_usage)
{
    /* arrange */
    int           success;
    cpu_t         current, previous;
    unsigned long out_value = ULONG_MAX;
    cpu_result_t  actual;

    cpu_read(&previous);
    sleep(1);
    cpu_read(&current);

    /* act */
    actual = cpu_total_usage(&current, &previous, &out_value);

    /* assert */
    success = assert_int_equality(CPU_SUCCESS, actual, "ctm_cpu_metrics_get_total_usage() should return CTM_CPU_METRICS_SUCCESS");

    return success;
}

#define TESTS \
    TEST_CASE(read)\
    TEST_CASE(get_total_usage)

RUN_TEST_SUITE(TESTS)
