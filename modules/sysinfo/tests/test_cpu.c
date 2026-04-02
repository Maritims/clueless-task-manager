//
// Created by martin on 28.03.2026.
//

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "ctm_cpu.h"

int assert_double_equal(const double expected, const double actual) {
    if (fabs(expected - actual) >= DBL_EPSILON) {
        printf("Assertion failed at %s:%d:\n"
               "\tExpected: %.16f\n"
               "\tActual  : %.16f\n",
               __FILE__,
               __LINE__,
               expected,
               actual);
    }
    return 0;
}

int ctm_read_file(const char* path, char* buffer, const size_t buffer_size) {
    strcpy(buffer, "cpu  9236370 6916 2562494 359278916 220954 0 720624 0 0 0");
    return 0;
}

void test_ctm_proc_stat_parse(void) {
    CtmCpuStats* stats = ctm_cpu_stats_from_kernel();

    assert(stats != NULL);
    assert(ctm_cpu_stats_get_user(stats) == 9236370);
    assert(ctm_cpu_stats_get_nice(stats) == 6916);
    assert(ctm_cpu_stats_get_system(stats) == 2562494);
    assert(ctm_cpu_stats_get_idle(stats) == 359278916);
    assert(ctm_cpu_stats_get_iowait(stats) == 220954);
    assert(ctm_cpu_stats_get_irq(stats) == 0);
    assert(ctm_cpu_stats_get_softirq(stats) == 720624);
    assert(ctm_cpu_stats_get_usage_scaled(stats) == 343);
    ctm_cpu_stats_free(stats);
    printf("%s passed\n", __func__);
}

int main(void) {
    test_ctm_proc_stat_parse();
    printf("All tests passed\n");
}
