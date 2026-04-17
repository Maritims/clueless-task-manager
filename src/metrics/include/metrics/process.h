/**
* @file process.h
 * @brief Portable API for retrieving and representing individual process metrics.
 *
 * This interface allows for querying specific process attributes and resource
 * consumption in a platform-agnostic manner.
 */

#ifndef CTM_PROCESS_H
#define CTM_PROCESS_H
#include <stddef.h>

/**
 * @brief Opaque handle representing a snapshot of a specific process's state.
 *
 * Internal members are implementation-defined to allow for cross-platform
 * compatibility.
 */
typedef struct Process Process;

/**
 * @brief Allocates memory for a new Process snapshot structure.
 * @retval Pointer to the allocated Process struct on success.
 * @retval NULL if memory allocation fails.
 */
Process* process_alloc(void);

/**
 * @brief Captures current metrics for a specific process into the provided structure.
 * @details The target process is typically identified by the PID already
 * stored within the structure or set via implementation-specific means.
 * @param process Pointer to an allocated Process structure.
 * @retval 0 on success.
 * @retval Non-zero error code if the process cannot be found or accessed.
 */
int process_capture(Process* process);

/**
 * @brief Convenience function that allocates and captures data for a specific PID.
 * @param pid The unique process identifier to query.
 * @retval Pointer to a populated Process struct on success.
 * @retval NULL if the process does not exist or allocation fails.
 */
Process* process_get(unsigned int pid);

/**
 * @brief Frees the memory associated with a process snapshot.
 * @param process Pointer to the Process structure to be deallocated.
 */
void process_free(Process* process);

/**
 * @brief Returns the size of the underlying Process structure.
 * @return The size of the struct in bytes.
 */
size_t process_size(void);

/**
 * @brief Retrieves the Process Identifier (PID) from the snapshot.
 * @param process The process snapshot to query.
 * @return The unsigned integer representing the PID.
 */
unsigned int process_get_pid(const Process* process);

/**
 * @brief Retrieves the command name or executable name of the process.
 * @param process The process snapshot to query.
 * @return A constant string containing the process name.
 */
const char* process_get_name(const Process* process);

/**
 * @brief Retrieves the current operational state of the process (e.g., Running, Sleeping).
 * @param process The process snapshot to query.
 * @return A constant string representing the process state.
 */
const char* process_get_state(const Process* process);

/**
 * @brief Retrieves the username of the owner of the process.
 * @param process The process snapshot to query.
 * @return A constant string representing the account name that owns the process.
 */
const char* process_get_user(const Process* process);

/**
 * @brief Retrieves the total CPU time consumed by the process.
 * @details This typically includes both user and system time accumulated
 * throughout the life of the process.
 * @param process The process snapshot to query.
 * @return Total process time in architecture-dependent ticks.
 */
unsigned int process_get_total_time(const Process* process);

#endif
