//
// Created by martin on 26.03.2026.
//

#ifndef CLUELESS_SYSINFO_H
#define CLUELESS_SYSINFO_H
#include <time.h>

#include "common.h"
#include "cpu.h"
#include "memory.h"

typedef struct {
    cpu_stats_t    cpu;
    memory_stats_t memory;
    time_t         timestamp;
} sysinfo_t;

stat_result_t sysinfo_fetch(sysinfo_t *sysinfo);

void sysinfo_free(sysinfo_t *sysinfo);

void sys_stats_print(const sysinfo_t *sysinfo, double cpu_load, data_unit_t unit);

#endif //CLUELESS_SYSINFO_H
