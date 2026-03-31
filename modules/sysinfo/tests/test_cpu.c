//
// Created by martin on 28.03.2026.
//

#include <assert.h>
#include <stdio.h>

#include "ctm_cpu.h"

void test_cpu_stats_new(void) {
    CtmCpuStats* stats = ctm_cpu_stats_new();
    assert(stats != NULL);
    assert(ctm_cpu_stats_get_user(stats) == 0);
    assert(ctm_cpu_stats_get_nice(stats) == 0);
    assert(ctm_cpu_stats_get_system(stats) == 0);
    assert(ctm_cpu_stats_get_idle(stats) == 0);
    assert(ctm_cpu_stats_get_iowait(stats) == 0);
    assert(ctm_cpu_stats_get_irq(stats) == 0);
    assert(ctm_cpu_stats_get_softirq(stats) == 0);
    assert(ctm_cpu_stats_get_total_sum(stats) == 0);
    ctm_cpu_stats_free(stats);
    printf("%s passed\n", __func__);
}

int main(void) {
    test_cpu_stats_new();
    printf("All tests passed\n");
}
