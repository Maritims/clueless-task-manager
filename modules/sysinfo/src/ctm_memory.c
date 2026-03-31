//
// Created by martin on 26.03.2026.
//

#include "ctm_memory.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct CtmMemoryStats {
    unsigned long available;
    unsigned long free;
    unsigned long total;
    double        used_percent;
};

CtmMemoryStats* ctm_memory_stats_new(void) {
    CtmMemoryStats* stats = malloc(sizeof(CtmMemoryStats));
    if (!stats) {
        fprintf(stderr, "Failed to allocate memory for memory stats: %d\n", errno);
        return NULL;
    }
    stats->available    = 0;
    stats->free         = 0;
    stats->total        = 0;
    stats->used_percent = 0.0;
    return stats;
}

void ctm_memory_stats_free(CtmMemoryStats *memory_stats) {
    if (memory_stats) {
        memory_stats->available    = 0;
        memory_stats->free         = 0;
        memory_stats->total        = 0;
        memory_stats->used_percent = 0.0;
        free(memory_stats);
    }
}

CtmMemoryStats *ctm_memory_stats_from_kernel(void) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        fprintf(stderr, "Failed to open /proc/meminfo: %s\n", strerror(errno));
        return NULL;
    }

    char            line[256];
    int             read_available = 0;
    int             read_free      = 0;
    int             read_total     = 0;
    CtmMemoryStats *memory_stats   = malloc(sizeof(CtmMemoryStats));
    if (!memory_stats) {
        fprintf(stderr, "Failed to allocate memory for memory stats: %d\n", errno);
        fclose(fp);
        return NULL;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemAvailable:", 13) == 0) {
            char *end               = NULL;
            errno                   = 0;
            memory_stats->available = strtoul(line + 13, &end, 10);
            if (errno == 0 && end != line + 13) read_available = 1;
        }
        if (strncmp(line, "MemFree:", 8) == 0) {
            char *end          = NULL;
            errno              = 0;
            memory_stats->free = strtoul(line + 8, &end, 10);
            if (errno == 0 && end != line + 8) read_free = 1;
        }
        if (strncmp(line, "MemTotal:", 9) == 0) {
            char *end           = NULL;
            errno               = 0;
            memory_stats->total = strtoul(line + 9, &end, 10);
            if (errno == 0 && end != line + 9) read_total = 1;
        }
    }

    fclose(fp);

    if (!read_available || !read_free || !read_total) {
        fprintf(stderr, "Failed to read /proc/meminfo: %s\n", strerror(errno));
        free(memory_stats);
        return NULL;
    }

    memory_stats->used_percent = memory_stats->total == 0 ? 0.0 : ((double) memory_stats->total - (double) memory_stats->available) / (double) memory_stats->total * 100.0;

    return memory_stats;
}

unsigned long ctm_memory_stats_get_available(CtmMemoryStats *stats) {
    if (!stats) {
        fprintf(stderr, "ctm_memory_stats_get_available: Invalid argument: NULL\n");
        return -1;
    }
    return stats->available;
}

unsigned long ctm_memory_stats_get_free(CtmMemoryStats *stats) {
    if (!stats) {
        fprintf(stderr, "ctm_memory_stats_get_free: Invalid argument: NULL\n");
        return -1;
    }
    return stats->free;
}

unsigned long ctm_memory_stats_get_total(CtmMemoryStats *stats) {
    if (!stats) {
        fprintf(stderr, "ctm_memory_stats_get_total: Invalid argument: NULL\n");
        return -1;
    }
    return stats->total;
}

double ctm_memory_stats_get_used_percent(CtmMemoryStats *stats) {
    if (!stats) {
        fprintf(stderr, "ctm_memory_stats_get_used_percent: Invalid argument: NULL\n");
        return -1;
    }
    return stats->used_percent;
}