/**
* @file cpu.h
 * @brief Portable API for representing and calculating CPU metrics.
 *
 * This interface defines the contract for CPU telemetry. While the current
 * backend may be Linux-specific, the API is designed to be platform-independent.
 */

#ifndef CTM_CPU_H
#define CTM_CPU_H

#include <stddef.h>

/**
 * @brief Opaque handle representing a snapshot of the CPU state.
 * * The internal layout of this structure is defined by the specific
 * platform implementation.
 */
typedef struct CPU CPU;

/**
 * @brief Allocates memory for a new CPU snapshot.
 * @retval Pointer to the allocated CPU struct on success.
 * @retval NULL if memory allocation fails.
 */
CPU* cpu_alloc(void);

/**
 * @brief Captures current system-wide CPU metrics into the provided structure.
 * @param cpu Pointer to an allocated CPU structure.
 * @retval 0 on success.
 * @retval Non-zero error code if the hardware/OS cannot be queried.
 */
int cpu_capture(CPU* cpu);

/**
 * @brief Convenience function that allocates and captures a CPU snapshot in one step.
 * @retval Pointer to a populated CPU struct on success.
 * @retval NULL if allocation or capture fails.
 */
CPU* cpu_get(void);

/**
 * @brief Frees the memory associated with a CPU snapshot.
 * @param cpu Pointer to the CPU structure to be deallocated.
 */
void cpu_free(CPU* cpu);

/**
 * @brief Returns the size of the underlying CPU structure.
 * @return The size of the struct in bytes.
 */
size_t cpu_size(void);

/**
 * @brief Retrieves the total accumulated CPU time (all states) from a snapshot.
 * @param cpu The CPU snapshot to query.
 * @return Total CPU time in architecture-dependent ticks.
 */
unsigned long cpu_get_total_time(const CPU* cpu);

/**
 * @brief Retrieves the total idle CPU time from a snapshot.
 * @param cpu The CPU snapshot to query.
 * @return Idle CPU time in architecture-dependent ticks.
 */
unsigned long cpu_get_idle_time(const CPU* cpu);

/**
 * @brief Calculates the user-mode CPU utilization between two snapshots.
 * @param current The more recent CPU snapshot.
 * @param previous The older CPU snapshot.
 * @return The utilization in milli-percent (e.g., 100000 = 100.000%).
 */
long cpu_get_user_usage(const CPU* current, const CPU* previous);

/**
 * @brief Calculates the system-mode (kernel) CPU utilization between two snapshots.
 * @param current The more recent CPU snapshot.
 * @param previous The older CPU snapshot.
 * @return The utilization in milli-percent (e.g., 5500 = 5.5%).
 */
long cpu_get_system_usage(const CPU* current, const CPU* previous);

/**
 * @brief Calculates the total aggregate CPU utilization between two snapshots.
 * @param current The more recent CPU snapshot.
 * @param previous The older CPU snapshot.
 * @return The utilization in milli-percent (1% = 1000).
 */
long cpu_get_total_usage(const CPU* current, const CPU* previous);

#endif
