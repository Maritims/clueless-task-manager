//
// Created by martin on 26.03.2026.
//

#include "sys_stats.h"

#include <stdio.h>
#include <time.h>

/**
 * Utility function for converting a value in kB to another unit.
 * @param kb_val The original value to convert
 * @param unit  The unit to convert to
 * @param suffix  The suffix to use
 * @return The converted value.
 */
static double convert_unit(const unsigned long kb_val, const data_unit_t unit, const char **suffix) {
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

stat_result_t sys_stats_fetch_all(sys_stats_t *stats) {
    if (!stats) {
        return STAT_ERR_OPEN;
    }

    stat_result_t memory_result = memory_fetch_stats(&stats->memory);
    if (memory_result != STAT_SUCCESS) {
        fprintf(stderr, "Failed to fetch memory stats\n");
        return memory_result;
    }

    stat_result_t cpu_result = cpu_fetch_stats(&stats->cpu);
    if (cpu_result != STAT_SUCCESS) {
        fprintf(stderr, "Failed to fetch CPU stats\n");
        return cpu_result;
    }

    stats->timestamp = time(NULL);

    return STAT_SUCCESS;
}

void sys_stats_print(const sys_stats_t *stats, const double cpu_load, data_unit_t unit) {
    if (!stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return;
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
}
