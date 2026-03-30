//
// Created by martin on 26.03.2026.
//

#include "ctm_cpu.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ctm_cpu_fetch_stats(CtmCpuStats *out_stats) {
    if (out_stats == NULL) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return EINVAL;
    }

    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) {
        fprintf(stderr, "OS error: %s\n", strerror(errno));
        return errno;
    }

    char buf[256];
    if (fgets(buf, sizeof(buf), fp)) {
        char *p = buf;

        if (strncmp(p, "cpu", 3) == 0) {
            p += 3;
            while (*p != ' ') p++;

            out_stats->user = strtoul(p, &p, 10);
            out_stats->nice = strtoul(p, &p, 10);
            out_stats->system = strtoul(p, &p, 10);
            out_stats->idle = strtoul(p, &p, 10);
            out_stats->iowait = strtoul(p, &p, 10);
            out_stats->irq = strtoul(p, &p, 10);
            out_stats->softirq = strtoul(p, &p, 10);
        }
    }
    fclose(fp);

    out_stats->total_sum = out_stats->user + out_stats->nice + out_stats->system + out_stats->idle + out_stats->iowait + out_stats->irq + out_stats->
                       softirq;

    return 0;
}

int ctm_cpu_calculate_load(const CtmCpuStats *prev, const CtmCpuStats *curr, double *out_load) {
    if (!prev || !curr) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return EINVAL;
    }

    const unsigned long total_difference = curr->total_sum - prev->total_sum;
    const unsigned long idle_difference = curr->idle - prev->idle;

    *out_load = total_difference == 0 ? 0.0 : (1.0 - (double) idle_difference / (double) total_difference) * 100.0;

    return 0;
}