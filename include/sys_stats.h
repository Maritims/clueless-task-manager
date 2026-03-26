//
// Created by martin on 26.03.2026.
//

#ifndef CLUELESS_TASK_MGR_STATS_H
#define CLUELESS_TASK_MGR_STATS_H
#include <time.h>

#include "cpu.h"
#include "memory.h"

/**
 * @struct sys_stats_t
 * @brief Unified system telemetry container.
 * * Composition allows us to pass a single pointer around instead of juggling multiple variables.
 */
typedef struct {
    cpu_stats_t cpu;
    memory_stats_t memory;
    time_t timestamp;
} sys_stats_t;

/**
 * Convenience function to refresh all system metrics at once.
 */
stat_result_t sys_stats_fetch_all(sys_stats_t *stats);

/**
 * Prints a formatted report of system stats to stdout.
 * @param stats The snapshot to print.
 * @param cpu_load The pre-calculated CPU percentage.
 * @param unit The unit to convert to.
 */
void sys_stats_print(const sys_stats_t *stats, double cpu_load, data_unit_t unit);

#endif //CLUELESS_TASK_MGR_STATS_H
