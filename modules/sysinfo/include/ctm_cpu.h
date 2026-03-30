//
// Created by martin on 28.03.2026.
//

#ifndef CTM_CPU_H
#define CTM_CPU_H

typedef struct {
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;
    unsigned long total_sum; // calculated
} CtmCpuStats;

int ctm_cpu_fetch_stats(CtmCpuStats *out_stats);

int ctm_cpu_calculate_load(const CtmCpuStats *prev, const CtmCpuStats *curr, double *out_load);

#endif //CTM_CPU_H
