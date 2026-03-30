//
// Created by martin on 28.03.2026.
//

#include <assert.h>

#include "ctm_memory.h"

int main(void) {
    CtmMemoryStats      stats  = {0};
    const int result = memory_fetch_stats(&stats);

    assert(result == 0);
    assert(stats.total > 0);
    assert(stats.available > 0);
    assert(stats.used_percent > 0);
    assert(stats.used_percent <= 100.0);
    assert(stats.used_percent >= 0.0);

    return 0;
}
