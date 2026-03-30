//
// Created by martin on 26.03.2026.
//

#ifndef CTM_SYSINFO_H
#define CTM_SYSINFO_H
#include <time.h>

#include "ctm_cpu.h"
#include "ctm_memory.h"
#include "ctm_process.h"

typedef struct {
    CtmCpuStats     cpu;
    CtmMemoryStats  memory;
    CtmProcessArray processes;
    time_t          timestamp;
} CtmSysStats;

int ctm_sysinfo_fetch(CtmSysStats *out_stats);

int ctm_sysinfo_free(CtmSysStats *sysinfo);

int ctm_sys_stats_print(const CtmSysStats *sysinfo, double cpu_load, CtmDataUnit unit);

#endif //CTM_SYSINFO_H