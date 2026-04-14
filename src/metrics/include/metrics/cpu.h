#ifndef CTM_CPU_H
#define CTM_CPU_H

#include <stddef.h>

typedef struct CPU CPU;

CPU*          cpu_alloc(void);
int           cpu_capture(CPU* cpu);
CPU*          cpu_get(void);
void          cpu_free(CPU* cpu);
size_t        cpu_size(void);
unsigned long cpu_get_total_time(const CPU* cpu);
unsigned long cpu_get_idle_time(const CPU* cpu);
long          cpu_get_user_usage(const CPU* current, const CPU* previous);
long          cpu_get_system_usage(const CPU* current, const CPU* previous);
/**
 * Calculates the total CPU usage.
 * @param current The newest CPU snapshot.
 * @param previous The previous CPU snapshot.
 * @return the result in "milli-percent" (1% = 1000)
 */
long cpu_get_total_usage(const CPU* current, const CPU* previous);

#endif
