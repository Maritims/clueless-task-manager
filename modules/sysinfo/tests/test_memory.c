//
// Created by martin on 28.03.2026.
//

#include <assert.h>
#include <stdio.h>

#include "ctm_memory.h"

void test_ctm_memory_stats_new(void) {
    CtmMemoryStats* stats = ctm_memory_stats_new();
    assert(stats != NULL);
    assert(ctm_memory_stats_get_available(stats) == 0);
    assert(ctm_memory_stats_get_free(stats) == 0);
    assert(ctm_memory_stats_get_total(stats) == 0);
    assert(ctm_memory_stats_get_used_percent(stats) == 0.0);
    ctm_memory_stats_free(stats);
    printf("%s passed\n", __func__);
}

int main(void) {
    test_ctm_memory_stats_new();
    printf("All tests passed\n");
}
