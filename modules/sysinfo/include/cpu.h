//
// Created by martin on 28.03.2026.
//

#ifndef CLUELESS_TASK_MGR_CPU_H
#define CLUELESS_TASK_MGR_CPU_H
#include "common.h"

typedef struct {
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;
    unsigned long total_sum; // calculated
} cpu_stats_t;

stat_result_t cpu_fetch_stats(cpu_stats_t *stats);

double cpu_calculate_load(const cpu_stats_t *prev, const cpu_stats_t *curr);

#endif //CLUELESS_TASK_MGR_CPU_H
