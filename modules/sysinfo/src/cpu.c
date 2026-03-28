//
// Created by martin on 26.03.2026.
//

#include "cpu.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

stat_result_t cpu_fetch_stats(cpu_stats_t *stats) {
    if (stats == NULL) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return STAT_ERR_OPEN;
    }

    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) {
        fprintf(stderr, "OS error: %s\n", strerror(errno));
        return STAT_ERR_OPEN;
    }

    char buf[256];
    if (fgets(buf, sizeof(buf), fp)) {
        char *p = buf;

        if (strncmp(p, "cpu", 3) == 0) {
            p += 3;
            while (*p != ' ') p++;

            stats->user = strtoul(p, &p, 10);
            stats->nice = strtoul(p, &p, 10);
            stats->system = strtoul(p, &p, 10);
            stats->idle = strtoul(p, &p, 10);
            stats->iowait = strtoul(p, &p, 10);
            stats->irq = strtoul(p, &p, 10);
            stats->softirq = strtoul(p, &p, 10);
        }
    }
    fclose(fp);

    stats->total_sum = stats->user + stats->nice + stats->system + stats->idle + stats->iowait + stats->irq + stats->
                       softirq;

    return STAT_SUCCESS;
}

double cpu_calculate_load(const cpu_stats_t *prev, const cpu_stats_t *curr) {
    if (!prev || !curr) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return 0.0;
    }

    const unsigned long total_difference = curr->total_sum - prev->total_sum;
    const unsigned long idle_difference = curr->idle - prev->idle;

    if (total_difference == 0) {
        return 0.0;
    }

    return (1.0 - ((double) idle_difference / (double) total_difference)) * 100.0;
}
