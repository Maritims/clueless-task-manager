//
// Created by martin on 28.03.2026.
//

#include <assert.h>

#include "memory.h"

int main(void) {
    memory_stats_t      stats  = {0};
    const stat_result_t result = memory_fetch_stats(&stats);

    assert(result == STAT_SUCCESS);
    assert(stats.total > 0);
    assert(stats.available > 0);
    assert(stats.used_percent > 0);
    assert(stats.used_percent <= 100.0);
    assert(stats.used_percent >= 0.0);

    return 0;
}
