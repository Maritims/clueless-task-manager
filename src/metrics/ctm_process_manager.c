#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "metrics/ctm_process_manager.h"
#include "core/list.h"
#include "internal/ctm_process_metrics_internal.h"

static ctm_process_manager_status_t mark_processes_inactive(const ctm_list_node_t* process_metrics_list) {
    ctm_list_node_t*       curr;
    ctm_process_metrics_t* process_metrics;

    if (process_metrics_list == NULL) {
        return CTM_PROCESS_MANAGER_ERR_INVALID_ARG;
    }

    ctm_list_for_each(curr, process_metrics_list) {
        process_metrics            = ctm_list_entry(curr, ctm_process_metrics_t, node);
        process_metrics->is_active = 0;
        curr                       = curr->next;
    }

    return CTM_PROCESS_MANAGER_SUCCESS;
}

static ctm_process_manager_status_t remove_inactive_processes(ctm_list_node_t* process_metrics_list, ProcessManagerCallback on_process_removed) {
    ctm_list_node_t*       curr;
    ctm_process_metrics_t* process_metrics;

    if (process_metrics_list == NULL) {
        return CTM_PROCESS_MANAGER_ERR_INVALID_ARG;
    }

    ctm_list_for_each(curr, process_metrics_list) {
        process_metrics = ctm_list_entry(curr, ctm_process_metrics_t, node);
        if (!process_metrics->is_active) {
            curr = curr->next;
            if (on_process_removed) {
                on_process_removed(process_metrics);
            }
            ctm_process_metrics_destroy(process_metrics);
        }
    }

    return CTM_PROCESS_MANAGER_SUCCESS;
}

static ctm_process_manager_status_t find_process_by_pid(const ctm_list_node_t* process_metrics_list, const unsigned int pid, ctm_process_metrics_t* out_value) {
    ctm_list_node_t*       curr;
    ctm_process_metrics_t* process_metrics;

    if (process_metrics_list == NULL || pid == 0 || out_value == NULL) {
        return CTM_PROCESS_MANAGER_ERR_INVALID_ARG;
    }

    ctm_list_for_each(curr, process_metrics_list) {
        process_metrics = ctm_list_entry(curr, ctm_process_metrics_t, node);
        if (process_metrics->pid == pid) {
            *out_value = *process_metrics;
            return CTM_PROCESS_MANAGER_SUCCESS;
        }
    }

    return CTM_PROCESS_MANAGER_ERR_NOT_FOUND;
}

ctm_process_manager_status_t process_manager_refresh(ctm_process_metrics_t*       process_metrics_list,
                                                     const size_t                 process_metrics_arr_len,
                                                     const ProcessManagerCallback on_process_added,
                                                     const ProcessManagerCallback on_process_updated,
                                                     const ProcessManagerCallback on_process_removed) {
    struct dirent* entry;
    DIR*           dir;

    if (process_metrics_list == NULL || process_metrics_arr_len == 0) {
        return CTM_PROCESS_MANAGER_ERR_INVALID_ARG;
    }

    if ((dir = opendir("/proc")) == NULL) {
        return CTM_PROCESS_MANAGER_ERR_OPEN;
    }

    /* Mark all processes as inactive */
    mark_processes_inactive(&process_metrics_list->node);

    /* Update existing processes and add new processes */
    while ((entry = readdir(dir)) != NULL) {
        long                  pid;
        char*                 end_ptr;
        unsigned int          upid;
        ctm_process_metrics_t existing_process_metrics;

        if (!isdigit((unsigned char) entry->d_name[0])) {
            continue;
        }

        errno = 0;
        pid   = strtol(entry->d_name, &end_ptr, 10);
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

        upid = (unsigned int) pid;
        if (find_process_by_pid(&process_metrics_list->node, upid, &existing_process_metrics) == CTM_PROCESS_MANAGER_SUCCESS) {
            const ctm_process_metrics_status_t process_metrics_read_status = ctm_process_metrics_read(upid, &existing_process_metrics);
            existing_process_metrics.is_active                             = (process_metrics_read_status == CTM_PROCESS_METRICS_SUCCESS);

            if (process_metrics_read_status == CTM_PROCESS_METRICS_SUCCESS) {
                if (on_process_updated) {
                    on_process_updated(&existing_process_metrics);
                }
            } else if (process_metrics_read_status != CTM_PROCESS_METRICS_SUCCESS) {
                /* Process was found by readdir but not capture, mark as inactive for removal */
                existing_process_metrics.is_active = 0;
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
            ctm_list_add(&new_process_metrics.node, &process_metrics_list->node);

            if (on_process_added) {
                on_process_added(&new_process_metrics);
            }
        }
    }
    closedir(dir);

    /* Remove inactive processes */
    remove_inactive_processes(&process_metrics_list->node, on_process_removed);

    return 0;
}
