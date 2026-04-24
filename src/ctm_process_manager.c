#include "ctm_process_manager.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ctm_types.h"
#include "ctm_process_metrics_internal.h"

static void mark_processes_inactive(ctm_process_metrics_t* process_metrics_arr) {
    ctm_process_metrics_t* curr = process_metrics_arr ? process_metrics_arr->node : NULL;
    while (curr) {
        curr->is_active = 0;
        curr            = curr->node;
    }
}

static void remove_inactive_processes(ctm_process_metrics_t** process_metrics_arr_ptr, ProcessManagerCallback on_process_removed) {
    while (*process_metrics_arr_ptr) {
        ctm_process_metrics_t* curr = container_of(*curr_ptr, Process, node);
        if (!curr->is_active) {
            ListNode* to_remove_node = *curr_ptr;
            *curr_ptr                = to_remove_node->next;

            if (on_process_removed) {
                on_process_removed(curr);
            }
            process_destroy(curr);
        } else {
            curr_ptr = &(*curr_ptr)->next;
        }
    }
}

static ctm_process_metrics_t* find_process_by_pid(ctm_process_metrics_t* process_metrics_arr, unsigned int pid) {
    ctm_process_metrics_t* curr = process_metrics_arr ? process_metrics_arr->node : NULL;
    while (curr) {
        if (curr->pid == pid) {
            return curr;
        }
        curr = curr->node;
    }
    return NULL;
}

ctm_process_manager_status_t process_manager_refresh(ctm_process_metrics_t*       proces_metrics_arr,
                                                     const size_t                 process_metrics_arr_len,
                                                     const ProcessManagerCallback on_process_added,
                                                     const ProcessManagerCallback on_process_updated,
                                                     const ProcessManagerCallback on_process_removed) {
    struct dirent* entry;
    DIR*           dir;

    if (proces_metrics_arr == NULL || process_metrics_arr_len == 0) {
        return CTM_PROCESS_MANAGER_ERR_INVALID_ARG;
    }

    if ((dir = opendir("/proc")) == NULL) {
        return CTM_PROCESS_MANAGER_ERR_OPEN;
    }

    /* Mark all processes as inactive */
    mark_processes_inactive(proces_metrics_arr);

    /* Update existing processes and add new processes */
    while ((entry = readdir(dir)) != NULL) {
        char* end_ptr;

        if (!isdigit((unsigned char) entry->d_name[0])) {
            continue;
        }

        errno    = 0;
        long pid = strtol(entry->d_name, &end_ptr, 10);
        if (*end_ptr != '\0' || end_ptr == entry->d_name) {
            continue;
        }
        if (errno == ERANGE || pid == LONG_MAX || pid == LONG_MIN) {
            closedir(dir);
            return -1;
        }
        if (pid == 0) {
            continue;
        }

        unsigned int           upid                     = (unsigned int) pid;
        ctm_process_metrics_t* existing_process_metrics = find_process_by_pid(proces_metrics_arr, upid);

        if (existing_process_metrics) {
            const ctm_process_metrics_status_t process_metrics_read_status = ctm_process_metrics_read(upid, existing_process_metrics);
            existing_process_metrics->is_active                            = (process_metrics_read_status == CTM_PROCESS_METRICS_SUCCESS);

            if (process_metrics_read_status == CTM_PROCESS_METRICS_SUCCESS) {
                if (on_process_updated) {
                    on_process_updated(existing_process_metrics);
                }
            } else if (process_metrics_read_status != CTM_PROCESS_METRICS_SUCCESS) {
                /* Process was found by readdir but not capture, mark as inactive for removal */
                existing_process_metrics->is_active = 0;
            } else {
                closedir(dir);
                return CTM_PROCESS_MANAGER_ERR_INTERNAL;
            }
        } else {
            ctm_process_metrics_t new_process_metrics;
            if (ctm_process_metrics_read(upid, &new_process_metrics) != CTM_PROCESS_METRICS_SUCCESS) {
                if (errno == ENOENT) {
                    continue;
                }

                closedir(dir);
                return CTM_PROCESS_MANAGER_ERR_INTERNAL;
            }

            /* Add to the front of the list */
            new_process->node  = proces_metrics_arr;
            proces_metrics_arr = new_process;

            if (on_process_added) {
                on_process_added(new_process);
            }
        }
    }
    closedir(dir);

    /* Remove inactive processes */
    remove_inactive_processes(&proces_metrics_arr, on_process_removed);

    return 0;
}
