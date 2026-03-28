//
// Created by martin on 28.03.2026.
//

#ifndef CLUELESS_TASK_MGR_MEMORY_H
#define CLUELESS_TASK_MGR_MEMORY_H
#include "common.h"

typedef enum {
    UNIT_KB = 0,
    UNIT_MB = 1,
    UNIT_GB = 2,
    UNIT_TB = 3
} data_unit_t;

typedef struct {
    unsigned long available;
    unsigned long free;
    unsigned long total;
    double        used_percent; // calculated
} memory_stats_t;

stat_result_t memory_fetch_stats(memory_stats_t *stats);

void memory_calculate_load(memory_stats_t *stats);

#endif //CLUELESS_TASK_MGR_MEMORY_H
