/**
* @file memory.h
 * @brief Portable API for representing and querying system memory metrics.
 *
 * This interface provides a platform-agnostic way to capture snapshots of
 * system memory state and retrieve key performance indicators.
 */

#ifndef MEM_METRICS_H
#define MEM_METRICS_H

/**
 * @brief Opaque handle representing a snapshot of the system memory state.
 *
 * The internal members are defined by the platform-specific implementation
 * to ensure portability across different operating systems.
 */
typedef struct Memory Memory;

/**
 * @brief Allocates and populates a new memory snapshot with current system data.
 * @retval Pointer to a populated Memory struct on success.
 * @retval NULL if allocation fails or the system metrics cannot be read.
 */
Memory* memory_get(void);

/**
 * @brief Frees the memory associated with a memory snapshot.
 * @param memory Pointer to the Memory structure to be deallocated.
 */
void memory_free(Memory* memory);

/**
 * @brief Retrieves the total physical RAM reported by the snapshot.
 * @param memory The memory snapshot to query.
 * @return Total physical memory in kilobytes (KB).
 */
unsigned long memory_get_total(const Memory* memory);

/**
 * @brief Retrieves the amount of completely    unused memory (free) from the snapshot.
 * @param memory The memory snapshot to query.
 * @return Free physical memory in kilobytes (KB).
 */
unsigned long memory_get_free(const Memory* memory);

/**
 * @brief Retrieves the amount of memory available for starting new applications.
 * @details This value typically accounts for memory that is currently used
 * by the OS cache but can be reclaimed immediately.
 * @param memory The memory snapshot to query.
 * @return Available physical memory in kilobytes (KB).
 */
unsigned long memory_get_available(const Memory* memory);

#endif
