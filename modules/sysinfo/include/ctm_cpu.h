//
// Created by martin on 28.03.2026.
//

#ifndef CTM_CPU_H
#define CTM_CPU_H

typedef struct CtmCpuStats CtmCpuStats;

// Constructor, constructor helpers and destructor.
CtmCpuStats* ctm_cpu_stats_new(void);
void         ctm_cpu_stats_free(CtmCpuStats* cpu_stats);

// Factory functions.
CtmCpuStats* ctm_cpu_stats_from_kernel(void);

// Accessors.
unsigned long ctm_cpu_stats_get_user(const CtmCpuStats* cpu_stats);
unsigned long ctm_cpu_stats_get_nice(const CtmCpuStats* cpu_stats);
unsigned long ctm_cpu_stats_get_system(const CtmCpuStats* cpu_stats);
unsigned long ctm_cpu_stats_get_idle(const CtmCpuStats* cpu_stats);
unsigned long ctm_cpu_stats_get_iowait(const CtmCpuStats* cpu_stats);
unsigned long ctm_cpu_stats_get_irq(const CtmCpuStats* cpu_stats);
unsigned long ctm_cpu_stats_get_softirq(const CtmCpuStats* cpu_stats);
unsigned int  ctm_cpu_stats_get_usage_scaled(const CtmCpuStats* cpu_stats);

#endif //CTM_CPU_H
