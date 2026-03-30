//
// Created by martin on 26.03.2026.
//

#include "ctm_memory.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int memory_fetch_stats(CtmMemoryStats *out_stats) {
    if (out_stats == NULL) {
        return EINVAL;
    }

    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        fprintf(stderr, "OS error: %s\n", strerror(errno));
        return errno;
    }

    char line[256];
    int read_available = 0;
    int read_free = 0;
    int read_total = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemAvailable:", 13) == 0) {
            char *end = NULL;
            errno = 0;
            out_stats->available = strtoul(line + 13, &end, 10);
            if (errno == 0 && end != line + 13) read_available = 1;
        }
        if (strncmp(line, "MemFree:", 8) == 0) {
            char *end = NULL;
            errno = 0;
            out_stats->free = strtoul(line + 8, &end, 10);
            if (errno == 0 && end != line + 8) read_free = 1;
        }
        if (strncmp(line, "MemTotal:", 9) == 0) {
            char *end = NULL;
            errno = 0;
            out_stats->total = strtoul(line + 9, &end, 10);
            if (errno == 0 && end != line + 9) read_total = 1;
        }
    }

    if (!read_available || !read_free || !read_total) {
        return EIO;
    }

    memory_calculate_load(out_stats);

    return 0;
}

int memory_calculate_load(CtmMemoryStats *out_stats) {
    if (out_stats == NULL) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return EINVAL;
    }

    const unsigned long total = out_stats->total;
    const unsigned long available = out_stats->available;

    out_stats->used_percent = total == 0 ? 0.0 : (total - available) / (double) total * 100.0;

    return 0;
}