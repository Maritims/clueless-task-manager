#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "process.h"
#include "internal/process_internal.h"

static process_result_t mark_processes_inactive(const list_node_t* process_node)
{
    list_node_t* curr;
    process_t*   process;

    if (process_node == NULL) {
        return PROCESS_ERR_INVALID_ARG;
    }

    LIST_FOREACH(curr, process_node) {
        process            = LIST_ENTRY(curr, process_t, node);
        process->is_active = 0;
    }

    return PROCESS_SUCCESS_READ;
}

static process_result_t remove_inactive_processes(const list_node_t*       process_node,
                                                  const process_callback_t on_process_removed)
{
    list_node_t* curr;
    list_node_t* n;
    process_t*   process;

    if (process_node == NULL) {
        return PROCESS_ERR_INVALID_ARG;
    }

    LIST_FOREACH_SAFE(curr, n, process_node) {
        process = LIST_ENTRY(curr, process_t, node);
        if (!process->is_active) {
            list_delete_node(curr);
            if (on_process_removed) {
                on_process_removed(process);
            }
            process_destroy(process);
        }
    }

    return PROCESS_SUCCESS_READ;
}

static process_result_t find_process_by_pid(const list_node_t* process_node,
                                            const unsigned int pid,
                                            process_t**        out)
{
    list_node_t* curr;
    process_t*   process;

    if (process_node == NULL || pid == 0 || out == NULL) {
        return PROCESS_ERR_INVALID_ARG;
    }

    LIST_FOREACH(curr, process_node) {
        process = LIST_ENTRY(curr, process_t, node);
        if (process->pid == pid) {
            *out = process;
            return PROCESS_SUCCESS_READ;
        }
    }

    return PROCESS_ERR_NOT_FOUND;
}

process_result_t process_list_refresh(list_node_t*             list,
                                      const size_t             len,
                                      const process_callback_t on_process_added,
                                      const process_callback_t on_process_updated,
                                      const process_callback_t on_process_removed)
{
    struct dirent* entry;
    DIR*           dir;

    (void) len;

    if (list == NULL) {
        return PROCESS_ERR_INVALID_ARG;
    }

    if ((dir = opendir("/proc")) == NULL) {
        return PROCESS_ERR_OPEN;
    }

    /* Mark all processes as inactive */
    mark_processes_inactive(list);

    /* Update existing processes and add new processes */
    while ((entry = readdir(dir)) != NULL) {
        long         pid;
        char*        end_ptr;
        unsigned int upid;
        process_t*   existing_process = NULL;

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
            return PROCESS_ERR_INTERNAL;
        }
        if (pid == 0) {
            continue;
        }

        upid = (unsigned int) pid;
        if (find_process_by_pid(list, upid, &existing_process) == PROCESS_SUCCESS_READ) {
            const process_result_t read_result = process_read(upid, existing_process);
            existing_process->is_active        = read_result == PROCESS_SUCCESS_READ;

            if (read_result == PROCESS_SUCCESS_READ) {
                if (on_process_updated) {
                    on_process_updated(existing_process);
                }
            } else if (read_result != PROCESS_SUCCESS_READ) {
                /* Process was found by readdir but not capture, mark as inactive for removal */
                existing_process->is_active = 0;
            } else {
                closedir(dir);
                return PROCESS_ERR_INTERNAL;
            }
        } else {
            process_t* new_process = calloc(1, sizeof(process_t));
            if (new_process == NULL) {
                closedir(dir);
                return PROCESS_ERR_INTERNAL;
            }

            if (process_read(upid, new_process) != PROCESS_SUCCESS_READ) {
                free(new_process);
                if (errno == ENOENT) {
                    continue;
                }

                closedir(dir);
                return PROCESS_ERR_INTERNAL;
            }

            new_process->pid       = upid;
            new_process->is_active = 1;

            /* Add to the front of the list */
            list_add_node(list, &new_process->node);

            if (on_process_added) {
                on_process_added(new_process);
            }
        }
    }
    closedir(dir);

    /* Remove inactive processes */
    remove_inactive_processes(list, on_process_removed);

    return PROCESS_SUCCESS_READ;
}
