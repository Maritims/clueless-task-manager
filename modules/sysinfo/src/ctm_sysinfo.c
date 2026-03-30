//
// Created by martin on 26.03.2026.
//

#include <stdio.h>
#include <time.h>

#include "ctm_cpu.h"
#include "ctm_memory.h"
#include "ctm_sysinfo.h"

#include <stdlib.h>
#include <string.h>
#include <asm-generic/errno-base.h>

/**
 * Utility function for converting a value in kB to another unit.
 * @param kb_val The original value to convert
 * @param unit  The unit to convert to
 * @param suffix  The suffix to use
 * @return The converted value.
 */
static double convert_unit(const unsigned long kb_val, const CtmDataUnit unit, const char **suffix) {
    switch (unit) {
        case UNIT_KB:
            *suffix = "kB";
            return (double) kb_val;
        case UNIT_MB:
            *suffix = "MB";
            return (double) kb_val / 1024.0;
        case UNIT_GB:
            *suffix = "GB";
            return (double) kb_val / 1024.0 / 1024.0;
        case UNIT_TB:
            *suffix = "TB";
            return (double) kb_val / 1024.0 / 1024.0 / 1024.0;
        default:
            fprintf(stderr, "Invalid unit: %d\n", unit);
            return (double) kb_val;
    }
}

int ctm_sysinfo_fetch(CtmSysStats *out_stats) {
    if (!out_stats) {
        fprintf(stderr, "ctm_sysinfo_fetch: Invalid argument: NULL\n");
        return EINVAL;
    }

    const int cpu_result = ctm_cpu_fetch_stats(&out_stats->cpu);
    if (cpu_result != 0) {
        fprintf(stderr, "Failed to fetch CPU stats\n");
        return cpu_result;
    }

    const int memory_result = memory_fetch_stats(&out_stats->memory);
    if (memory_result != 0) {
        fprintf(stderr, "Failed to fetch memory stats: %s\n", strerror(memory_result));
        return memory_result;
    }

    const int process_result = ctm_process_fetch_all(&out_stats->processes, 1);
    if (process_result != 0) {
        fprintf(stderr, "Failed to fetch process stats: %s\n", strerror(process_result));
        return process_result;
    }

    out_stats->timestamp = time(NULL);

    return 0;
}

int ctm_sysinfo_free(CtmSysStats *sysinfo) {
    if (!sysinfo) {
        fprintf(stderr, "ctm_sysinfo_free: Invalid argument: NULL\n");
        return EINVAL;
    }

    free(sysinfo);
    return 0;
}

int ctm_sys_stats_print(const CtmSysStats *stats, const double cpu_load, CtmDataUnit unit) {
    if (!stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return EINVAL;
    }

    const char *unit_str;
    const double total = convert_unit(stats->memory.total, unit, &unit_str);
    const double available = convert_unit(stats->memory.available, unit, &unit_str);
    const double used = convert_unit(stats->memory.total - stats->memory.available, unit, &unit_str);

    printf("\n[            System stats            ]\n");
    printf("--------------------------------------\n");

    printf("Memory Usage (%s):\n", unit_str);
    printf("  Total     : %8.2f %s\n", total, unit_str);
    printf("  Available : %8.2f %s\n", available, unit_str);
    printf("  Used      : [%6.1f%%]\n", stats->memory.used_percent);

    printf("CPU:\n");
    printf("  Load      : [%6.1f%%]\n", cpu_load);

    int bar_width = 20;
    int pos = (int) ((cpu_load / 100.0) * bar_width);
    printf("  Visual    : [");
    for (int i = 0; i < bar_width; i++) {
        if (i < pos) printf("#");
        else printf(".");
    }
    printf("]\n");

    printf("--------------------------------------\n");

    return 0;
}
