/**
* @file process_list.h
 * @brief API for managing and observing a collection of system processes.
 *
 * This header provides structures and functions to maintain a dynamic list of
 * processes and monitor changes over time using an observer pattern.
 */

#ifndef CTM_PROCESS_LIST_H
#define CTM_PROCESS_LIST_H

#include <stddef.h>

#include "process.h"

/**
 * @brief Opaque handle representing a collection of process snapshots.
 */
typedef struct ProcessList ProcessList;

/**
 * @brief Callback function type for iterating over a ProcessList.
 * @param process A pointer to the current Process entry in the iteration.
 * @retval 0 to continue iteration.
 * @retval Non-zero to stop iteration.
 */
typedef int (*ProcessListForeachFunc)(Process* process);

/**
 * @brief Observer structure containing hooks for process lifecycle events.
 * * When passed to process_list_refresh(), these callbacks are triggered based
 * on the delta between the current system state and the previous refresh.
 */
typedef struct ProcessListObserver {
    /** Called when a new PID is detected in the system. */
    void (*on_process_added)(Process* process, void* user_data);
    /** Called when an existing process is updated with new metrics. */
    void (*on_process_updated)(Process* process, void* user_data);
    /** Called when a process is no longer present in the system. */
    void (*on_process_removed)(Process* process, void* user_data);
} ProcessListObserver;

/**
 * @brief Allocates memory for a new ProcessList container.
 * @retval Pointer to the allocated ProcessList on success.
 * @retval NULL if memory allocation fails.
 */
ProcessList* process_list_alloc(void);

/**
 * @brief Frees the ProcessList and all internally managed Process snapshots.
 * @param list Pointer to the ProcessList to be deallocated.
 */
void process_list_free(ProcessList* list);

/**
 * @brief Synchronizes the ProcessList with the current system state.
 * @details This function iterates through active system processes, updates internal
 * state, and triggers the provided observer callbacks for any changes.
 * * @param list The ProcessList to refresh.
 * @param observer Pointer to a ProcessListObserver containing callbacks (can be NULL).
 * @param user_data Context pointer passed to the observer callbacks.
 * @param out_size Optional pointer to receive the total number of processes after refresh.
 * @retval 0 on success.
 * @retval Non-zero error code if the system process table cannot be read.
 */
int process_list_refresh(ProcessList* list, ProcessListObserver* observer, void* user_data, size_t* out_size);

/**
 * @brief Returns the number of processes currently tracked in the list.
 * @param list The ProcessList to query.
 * @return The total count of processes.
 */
size_t process_list_get_count(const ProcessList* list);

/**
 * @brief Locates a specific process in the list by its PID.
 * @param list The ProcessList to search.
 * @param pid The process identifier to find.
 * @retval Pointer to the Process snapshot if found.
 * @retval NULL if the PID is not present in the list.
 */
Process* process_list_find_by_pid(const ProcessList* list, unsigned int pid);

/**
 * @brief Iterates over every process in the list, calling a function for each.
 * @param list The ProcessList to iterate over.
 * @param foreach_func The callback to execute for each process.
 * @retval 0 if the entire list was traversed.
 * @retval The non-zero value returned by foreach_func that halted iteration.
 */
int process_list_foreach(const ProcessList* list, ProcessListForeachFunc foreach_func);

#endif
