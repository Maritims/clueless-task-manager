#ifndef CLUELESS_TASK_MGR_CPU_H
#define CLUELESS_TASK_MGR_CPU_H

/**
 * @file cpu.h
 * @brief Generic CPU data acquisition for Linux-based systems.
 */

#include "common.h"

/**
 * CPU snapshot data.
 * Represents raw 'ticks' from the kernel.
 */
typedef struct {
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;

    unsigned long total_sum;
} cpu_stats_t;

/**
 * Fetches the current CPU ticks from /proc/stat.
 * @param stats Pointer to a pre-allocated {@link cpu_stats_t}.
 * @return STAT_SUCCESS (0) on success, or an error code.
 */
stat_result_t cpu_fetch_stats(cpu_stats_t *stats);

/**
 * Calculates the CPU load percentage between two snapshots.
 * @param prev The previous snapshot.
 * @param curr The current snapshot.
 * @return CPU load as a percentage (0.0 to 100.0).
 */
double cpu_calculate_load(const cpu_stats_t *prev, const cpu_stats_t *curr);

#endif // CLUELESS_TASK_MGR_CPU_H