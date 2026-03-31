//
// Created by martin on 26.03.2026.
//

#include <stdio.h>
#include <time.h>

#include "ctm_cpu.h"
#include "ctm_memory.h"
#include "ctm_system.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct CtmSystemStats {
    CtmCpuStats*     cpu;
    CtmMemoryStats*  memory;
    CtmProcessArray* processes;
    time_t           timestamp;
};

CtmSystemStats* ctm_system_stats_new(void) {
    CtmSystemStats* stats = malloc(sizeof(CtmSystemStats));
    if (!stats) {
        fprintf(stderr, "Failed to allocate memory for system stats: %d\n", errno);
        return NULL;
    }

    stats->cpu       = NULL;
    stats->memory    = NULL;
    stats->processes = NULL;
    stats->timestamp = time(NULL);

    return stats;
}

void ctm_system_stats_free(CtmSystemStats* system_stats) {
    if (system_stats) {
        ctm_cpu_stats_free(system_stats->cpu);
        ctm_memory_stats_free(system_stats->memory);
        ctm_process_array_free(system_stats->processes);
        system_stats->timestamp = 0;
        free(system_stats);
    }
}

CtmSystemStats* ctm_system_stats_from_kernel(void) {
    CtmSystemStats* system_stats = ctm_system_stats_new();

    system_stats->cpu       = ctm_cpu_stats_from_kernel();
    system_stats->memory    = ctm_memory_stats_from_kernel();
    system_stats->processes = ctm_processes_from_kernel(1);
    system_stats->timestamp = time(NULL);

    if (!system_stats->cpu || !system_stats->memory || !system_stats->processes) {
        fprintf(stderr, "Failed to fetch system stats\n");
        ctm_system_stats_free(system_stats);
        return NULL;
    }

    return system_stats;
}

CtmCpuStats* ctm_system_stats_get_cpu(const CtmSystemStats* system_stats) {
    if (!system_stats) {
        fprintf(stderr, "ctm_system_stats_get_cpu: stats cannot be NULL\n");
        return NULL;
    }
    return system_stats->cpu;
}

CtmMemoryStats* ctm_system_stats_get_memory(const CtmSystemStats* system_stats) {
    if (!system_stats) {
        fprintf(stderr, "ctm_system_stats_get_memory: stats cannot be NULL\n");
        return NULL;
    }
    return system_stats->memory;
}

CtmProcessArray* ctm_system_stats_get_processes(const CtmSystemStats* system_stats) {
    if (!system_stats) {
        fprintf(stderr, "ctm_system_stats_get_processes: stats cannot be NULL\n");
        return NULL;
    }
    return system_stats->processes;
}

time_t ctm_system_stats_get_timestamp(const CtmSystemStats* system_stats) {
    if (!system_stats) {
        fprintf(stderr, "ctm_system_get_timestamp: stats cannot be NULL\n");
        return 0;
    }
    return system_stats->timestamp;
}