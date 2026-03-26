//
// Created by martin on 26.03.2026.
//

#ifndef CLUELESS_TASK_MGR_MEMORY_H
#define CLUELESS_TASK_MGR_MEMORY_H

/**
 * @file memory.h
 * @brief Memory data acquisition for Linux-based systems.
 */

#include "common.h"

/**
 * Point-in-time snapshot of system memory utilization.
 * Units are in kilobytes (kB).
 */
typedef struct {
    unsigned long available;
    unsigned long free;
    unsigned long total;
    double used_percent;
} memory_stats_t;

/**
 * Populates a memory stats structure with current kernel data.
 * @param stats Pointer to a pre-allocated struct.
 * @return
 */
stat_result_t memory_fetch_stats(memory_stats_t *stats);

/**
 * Calculates the memory usage percentage.
 * @param stats Pointer to a pre-allocated struct.
 * @return
 */
void memory_calculate_load(memory_stats_t *stats);

#endif // CLUELESS_TASK_MGR_MEMORY_H