//
// Created by martin on 28.03.2026.
//

#include <assert.h>
#include <stdio.h>

#include "cpu.h"

int main(void) {
    cpu_stats_t stats = {0};
    const stat_result_t result = cpu_fetch_stats(&stats);

    assert(result == STAT_SUCCESS);
    assert(stats.user > 0);
    assert(stats.nice > 0);
    assert(stats.system > 0);
    assert(stats.idle > 0);
    assert(stats.iowait > 0);
    assert(stats.softirq > 0);
    assert(stats.total_sum == stats.user + stats.nice + stats.system + stats.idle + stats.iowait + stats.irq + stats.softirq);

    return 0;
}
