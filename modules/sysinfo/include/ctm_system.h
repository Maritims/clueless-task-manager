//
// Created by martin on 26.03.2026.
//

#ifndef CTM_SYSINFO_H
#define CTM_SYSINFO_H
#include <time.h>

#include "ctm_cpu.h"
#include "ctm_memory.h"
#include "ctm_process.h"

typedef struct CtmSystemStats CtmSystemStats;

// Constructor and destructor.
CtmSystemStats* ctm_system_stats_new(void);
void            ctm_system_stats_free(CtmSystemStats* system_stats);


// Factory functions.
CtmSystemStats* ctm_system_stats_from_kernel(void);

// Accessors.
CtmCpuStats*     ctm_system_stats_get_cpu(const CtmSystemStats* system_stats);
CtmMemoryStats*  ctm_system_stats_get_memory(const CtmSystemStats* system_stats);
CtmProcessArray* ctm_system_stats_get_processes(const CtmSystemStats* system_stats);
time_t           ctm_system_stats_get_timestamp(const CtmSystemStats* sys_stats);

#endif //CTM_SYSINFO_H
