//
// Created by martin on 26.03.2026.
//

#include "ctm_cpu.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct CtmCpuStats {
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;
    unsigned long total_sum; // calculated
};

CtmCpuStats* ctm_cpu_stats_new(void) {
    CtmCpuStats* stats = malloc(sizeof(CtmCpuStats));
    if (!stats) {
        fprintf(stderr, "Failed to allocate memory for CPU stats: %d\n", errno);
        return NULL;
    }
    stats->user      = 0;
    stats->nice      = 0;
    stats->system    = 0;
    stats->idle      = 0;
    stats->iowait    = 0;
    stats->irq       = 0;
    stats->softirq   = 0;
    stats->total_sum = 0;
    return stats;
}

void ctm_cpu_stats_free(CtmCpuStats* cpu_stats) {
    if (cpu_stats) {
        free(cpu_stats);
    }
}

CtmCpuStats* ctm_cpu_stats_from_kernel(void) {
    FILE* fp = fopen("/proc/stat", "r");
    if (!fp) {
        fprintf(stderr, "OS error: %s\n", strerror(errno));
        return NULL;
    }

    char buf[256];
    if (!fgets(buf, sizeof(buf), fp)) {
        fprintf(stderr, "Failed to read /proc/stat: %s\n", strerror(errno));
        fclose(fp);
        return NULL;
    }
    fclose(fp);

    char* p = buf;
    if (strncmp(p, "cpu", 3) != 0) {
        fprintf(stderr, "Invalid /proc/stat format: %s\n", buf);
        return NULL;
    }

    p += 3;
    while (*p != ' ') {
        p++;
    }

    CtmCpuStats* cpu_stats = ctm_cpu_stats_new();
    if (!cpu_stats) {
        fprintf(stderr, "Failed to allocate memory for CPU stats: %d\n", errno);
        return NULL;
    }
    cpu_stats->user    = strtoul(p, &p, 10);
    cpu_stats->nice    = strtoul(p, &p, 10);
    cpu_stats->system  = strtoul(p, &p, 10);
    cpu_stats->idle    = strtoul(p, &p, 10);
    cpu_stats->iowait  = strtoul(p, &p, 10);
    cpu_stats->irq     = strtoul(p, &p, 10);
    cpu_stats->softirq = strtoul(p, &p, 10);
    fclose(fp);

    cpu_stats->total_sum = cpu_stats->user + cpu_stats->nice + cpu_stats->system + cpu_stats->idle + cpu_stats->iowait + cpu_stats->irq + cpu_stats->
                           softirq;

    return cpu_stats;
}

unsigned long ctm_cpu_stats_get_user(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->user;
}

unsigned long ctm_cpu_stats_get_nice(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->nice;
}

unsigned long ctm_cpu_stats_get_system(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->system;
}

unsigned long ctm_cpu_stats_get_idle(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->idle;
}

unsigned long ctm_cpu_stats_get_iowait(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->iowait;
}

unsigned long ctm_cpu_stats_get_irq(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->irq;
}

unsigned long ctm_cpu_stats_get_softirq(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->softirq;
}

unsigned long ctm_cpu_stats_get_total_sum(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->total_sum;
}

double ctm_cpu_stats_get_load_delta(const CtmCpuStats* previous, const CtmCpuStats* current) {
    if (!previous || !current) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }

    const unsigned long total_difference = current->total_sum - previous->total_sum;
    const unsigned long idle_difference  = current->idle - previous->idle;

    return total_difference == 0 ? 0.0 : (1.0 - (double) idle_difference / (double) total_difference) * 100.0;
}
