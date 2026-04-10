#include "ctm/ctm.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ProcessList {
    HashMap* pid_map;
};

typedef struct ProcessEntry {
    Process* process;
    bool     active;
} ProcessEntry;

ProcessList* process_list_alloc(void) {
    ProcessList* process_list = malloc(sizeof(ProcessList));
    if (process_list == NULL) {
        return NULL;
    }

    process_list->pid_map = hash_map_create(sizeof(unsigned int), sizeof(ProcessEntry), hash_int, hash_compare_int);
    if (process_list->pid_map == NULL) {
        process_list_free(process_list);
        return NULL;
    }

    return process_list;
}

void process_list_free(ProcessList* list) {
    if (list) {
        hash_map_free(list->pid_map);
        free(list);
    }
}

size_t process_list_refresh(ProcessList* list) {
    struct dirent* entry;
    DIR*           dir;

    if (list == NULL) {
        errno = EINVAL;
        return 0;
    }
    if ((dir = opendir("/proc")) == NULL) {
        return 0;
    }

    while ((entry = readdir(dir)) != NULL) {
        ProcessEntry* existing_process_entry;
        char*         end_ptr;
        long          pid;

        if (!isdigit(entry->d_name[0])) {
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
            errno = ERANGE;
            return 0;
        }
        if (pid == 0) {
            /* Not a pid directory */
            continue;
        }

        existing_process_entry = hash_map_get(list->pid_map, &pid);
        if (existing_process_entry) {
            existing_process_entry->active = process_capture(existing_process_entry->process) == 0 ? true : false;
        } else {
            ProcessEntry new_process_entry;

            new_process_entry.active  = true;
            new_process_entry.process = process_get(pid);
            if (new_process_entry.process == NULL) {
                fprintf(stderr, "process_list_refresh: Failed to capture process %lu: %s\n", pid, strerror(errno));
                return 0;
            }

            if (hash_map_put(list->pid_map, &pid, &new_process_entry) != 0) {
                const int error = errno;
                process_free(new_process_entry.process);
                errno = error;
                return 0;
            }
        }
    }
    closedir(dir);

    return hash_map_count(list->pid_map);
}

size_t process_list_get_count(const ProcessList* list) {
    if (list == NULL) {
        errno = EINVAL;
        return 0;
    }
    return hash_map_count(list->pid_map);
}

