//
// Created by martin on 28.03.2026.
//

#include <assert.h>
#include <stdio.h>

#include "ctm_cpu.h"

int main(void) {
    CtmCpuStats stats = {0};
    const int result = ctm_cpu_fetch_stats(&stats);

    assert(result == 0);
    assert(stats.user > 0);
    assert(stats.nice > 0);
    assert(stats.system > 0);
    assert(stats.idle > 0);
    assert(stats.iowait > 0);
    assert(stats.softirq > 0);
    assert(stats.total_sum == stats.user + stats.nice + stats.system + stats.idle + stats.iowait + stats.irq + stats.softirq);

    return 0;
}
