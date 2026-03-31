//
// Created by martin on 28.03.2026.
//

#include <assert.h>
#include <stdio.h>

#include "ctm_system.h"

void test_ctm_sys_stats_new(void) {
    CtmSystemStats* system_stats = ctm_system_stats_new();
    assert(system_stats != NULL);
    assert(ctm_system_stats_get_cpu(system_stats) == NULL);
    assert(ctm_system_stats_get_memory(system_stats) == NULL);
    assert(ctm_system_stats_get_processes(system_stats) == NULL);
    assert(ctm_system_stats_get_timestamp(system_stats) > 0);
    ctm_system_stats_free(system_stats);
    printf("%s passed\n", __func__);
}

int main(void) {
    test_ctm_sys_stats_new();
    printf("All tests passed\n");
}
