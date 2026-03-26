//
// Created by martin on 26.03.2026.
//

#include "memory.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

stat_result_t memory_fetch_stats(memory_stats_t *stats) {
    if (stats == NULL) {
        return STAT_ERR_OPEN;
    }

    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        fprintf(stderr, "OS error: %s\n", strerror(errno));
        return STAT_ERR_OPEN;
    }

    char line[256];
    int read_available = 0;
    int read_free = 0;
    int read_total = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemAvailable:", 13) == 0) {
            char *end = NULL;
            errno = 0;
            stats->available = strtoul(line + 13, &end, 10);
            if (errno == 0 && end != line + 13) read_available = 1;
        }
        if (strncmp(line, "MemFree:", 8) == 0) {
            char *end = NULL;
            errno = 0;
            stats->free = strtoul(line + 8, &end, 10);
            if (errno == 0 && end != line + 8) read_free = 1;
        }
        if (strncmp(line, "MemTotal:", 9) == 0) {
            char *end = NULL;
            errno = 0;
            stats->total = strtoul(line + 9, &end, 10);
            if (errno == 0 && end != line + 9) read_total = 1;
        }
    }

    if (!read_available || !read_free || !read_total) {
        return STAT_ERR_PARSE;
    }

    memory_calculate_load(stats);

    return STAT_SUCCESS;
}

void memory_calculate_load(memory_stats_t *stats) {
    if (stats == NULL) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return;
    }

    const unsigned long total = stats->total;
    const unsigned long available = stats->available;

    if (total == 0) {
        stats->used_percent = 0.0;
        return;
    }

    const double usage_percent = ((total - available) / (double) total) * 100.0;
    stats->used_percent = usage_percent;
}
