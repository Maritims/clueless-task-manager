#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ctm/ctm.h"

struct ProcessList {
    unsigned int* pids;
    size_t        count;
    size_t        capacity;
};

ProcessList* process_list_get(void) {
    ProcessList*   process_list;
    struct dirent* entry;
    DIR*           dir;

    if ((dir = opendir("/proc")) == NULL) {
        return NULL;
    }
    if ((process_list = malloc(sizeof(ProcessList))) == NULL) {
        closedir(dir);
        fprintf(stderr, "process_list_get: Failed to allocate memory for process list: %s\n", strerror(errno));
        return NULL;
    }

    process_list->count    = 0;
    process_list->capacity = 100;
    if ((process_list->pids = malloc(process_list->capacity * sizeof(unsigned int))) == NULL) {
        closedir(dir);
        fprintf(stderr, "process_list_get: Failed to allocate memory for process list: %s\n", strerror(errno));
        free(process_list);
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        char* end_ptr;
        long  pid;

        if (entry->d_type != DT_DIR) {
            /* The entry is not a directory */
            continue;
        }

        errno = 0;
        pid   = strtol(entry->d_name, &end_ptr, 10);
        if (*end_ptr != '\0' || end_ptr == entry->d_name) {
            /* The directory name contained a number and something else */
            continue;
        }
        if (pid == LONG_MAX || pid == LONG_MIN) {
            /* An underflow or an overflow occurred */
            fprintf(stderr, "process_list_get: Failed to parse pid from /proc/%s: %s\n", entry->d_name, strerror(errno));
            process_list_free(process_list);
            return NULL;
        }
        if (pid == 0) {
            /* Not a pid directory */
            continue;
        }

        if (process_list->count == process_list->capacity) {
            unsigned int* new_pids;

            process_list->capacity *= 2;
            new_pids               = realloc(process_list->pids, process_list->capacity * sizeof(unsigned int));
            if (new_pids == NULL) {
                fprintf(stderr, "process_list_get: Failed to allocate memory for process list: %s\n", strerror(errno));
                process_list_free(process_list);
                return NULL;
            }

            process_list->pids = new_pids;
        }
        process_list->pids[process_list->count++] = (unsigned int) pid;
    }

    closedir(dir);
    return process_list;
}

void process_list_free(ProcessList* process_list) {
    if (process_list) {
        free(process_list->pids);
        free(process_list);
    }
}

size_t process_list_get_count(const ProcessList* process_list) {
    if (process_list == NULL) {
        fprintf(stderr, "process_list_get_count: Invalid argument: NULL\n");
        return 0;
    }
    return process_list->count;
}

Process* process_list_get_process(const ProcessList* process_list, const unsigned int pid) {
    size_t i;

    if (process_list == NULL) {
        fprintf(stderr, "process_list_get_process: Invalid argument: NULL\n");
        return NULL;
    }

    for (i = 0; i < process_list->count; i++) {
        if (process_list->pids[i] == pid) {
            return process_get(pid);
        }
    }

    return NULL;
}
