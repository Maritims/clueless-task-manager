//
// Created by martin on 28.03.2026.
//

#ifndef CTM_MEMORY_H
#define CTM_MEMORY_H

typedef enum {
    UNIT_KB = 0,
    UNIT_MB = 1,
    UNIT_GB = 2,
    UNIT_TB = 3
} CtmDataUnit;

typedef struct {
    unsigned long available;
    unsigned long free;
    unsigned long total;
    double        used_percent; // calculated
} CtmMemoryStats;

int memory_fetch_stats(CtmMemoryStats *out_stats);

int memory_calculate_load(CtmMemoryStats *out_stats);

#endif //CTM_MEMORY_H
