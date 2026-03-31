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

typedef struct CtmMemoryStats CtmMemoryStats;

// Constructor and destructor.
CtmMemoryStats* ctm_memory_stats_new(void);
void            ctm_memory_stats_free(CtmMemoryStats* memory_stats);

// Factory functions.
CtmMemoryStats* ctm_memory_stats_from_kernel(void);

// Accessors.
unsigned long ctm_memory_stats_get_available(CtmMemoryStats* memory_stats);
unsigned long ctm_memory_stats_get_free(CtmMemoryStats* memory_stats);
unsigned long ctm_memory_stats_get_total(CtmMemoryStats* memory_stats);
double        ctm_memory_stats_get_used_percent(CtmMemoryStats* memory_stats);

#endif //CTM_MEMORY_H
