#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "collections/array.h"
#include "collections/hash_map.h"
#include "metrics/process.h"
#include "metrics/process_list.h"

struct ProcessList {
    HashMap* process_map;
};

typedef struct ProcessEntry {
    Process*     process;
    unsigned int active;
} ProcessEntry;

ProcessList* process_list_alloc(void) {
    ProcessList* process_list = malloc(sizeof(ProcessList));
    if (process_list == NULL) {
        return NULL;
    }

    process_list->process_map = hash_map_create(sizeof(unsigned int), sizeof(ProcessEntry), hash_int, hash_compare_int);
    if (process_list->process_map == NULL) {
        process_list_free(process_list);
        return NULL;
    }

    return process_list;
}

void process_list_free(ProcessList* list) {
    if (list) {
        hash_map_free(list->process_map);
        free(list);
    }
}

static void mark_processes_inactive(HashMap* process_map) {
    void *       key, *value;
    HashMapIter* iter = hash_map_iter_create(process_map);

    while (hash_map_iter_next(iter, &key, &value) == 0) {
        ProcessEntry* process_entry = value;
        process_entry->active       = 0;
    }

    hash_map_iter_free(iter);
}

static void remove_inactive_processes(HashMap* process_map, ProcessListObserver* observer, void* user_data) {
    HashMapIter* iter;
    void *       key, *value;
    Array*       keys_to_remove;
    size_t       i;

    iter           = hash_map_iter_create(process_map);
    keys_to_remove = array_create();
    while (hash_map_iter_next(iter, &key, &value) == 0) {
        ProcessEntry* process_entry = value;
        if (!process_entry->active) {
            array_add(keys_to_remove, key);
        }
    }
    hash_map_iter_free(iter);

    for (i = 0; i < array_count(keys_to_remove); i++) {
        const void*         pid_key       = array_get(keys_to_remove, i);
        const ProcessEntry* process_entry = hash_map_get(process_map, pid_key);

        if (observer && observer->on_process_removed) {
            observer->on_process_removed(process_entry->process, user_data);
        }

        if (process_entry) {
            process_free(process_entry->process);
        }
        hash_map_remove(process_map, pid_key);
    }
    array_free(keys_to_remove);
}

int process_list_refresh(ProcessList* list, ProcessListObserver* observer, void* user_data, size_t* out_size) {
    struct dirent* entry;
    DIR*           dir;

    if (list == NULL) {
        errno = EINVAL;
        return -1;
    }
    if ((dir = opendir("/proc")) == NULL) {
        return -1;
    }

    /*  Mark all processes as inactive */
    mark_processes_inactive(list->process_map);

    /* Update existing processes and add new processes */
    while ((entry = readdir(dir)) != NULL) {
        ProcessEntry* existing_process_entry;
        char*         end_ptr;
        long          pid;
        unsigned int  upid;

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
            closedir(dir);
            return -1;
        }
        if (pid == 0) {
            /* Not a pid directory */
            continue;
        }

        upid                   = (unsigned int) pid;
        existing_process_entry = hash_map_get(list->process_map, &upid);
        if (existing_process_entry) {
            existing_process_entry->active = process_capture(existing_process_entry->process) == 0 ? 1 : 0;
            if (observer && observer->on_process_updated) {
                observer->on_process_updated(existing_process_entry->process, user_data);
            }
        } else {
            ProcessEntry new_process_entry;

            new_process_entry.active  = 1;
            new_process_entry.process = process_get(upid);
            if (new_process_entry.process == NULL) {
                fprintf(stderr, "process_list_refresh: Failed to capture process %d: %s\n", upid, strerror(errno));
                return -1;
            }

            if (hash_map_put(list->process_map, &upid, &new_process_entry) != 0) {
                const int error = errno;
                process_free(new_process_entry.process);
                errno = error;
                return -1;
            }

            if (observer && observer->on_process_added) {
                observer->on_process_added(new_process_entry.process, user_data);
            }
        }
    }
    closedir(dir);

    /* Remove inactive processes */
    remove_inactive_processes(list->process_map, observer, user_data);

    *out_size = hash_map_count(list->process_map);
    return 0;
}

size_t process_list_get_count(const ProcessList* list) {
    if (list == NULL) {
        errno = EINVAL;
        return 0;
    }
    return hash_map_count(list->process_map);
}

int process_list_foreach(const ProcessList* list, const ProcessListForeachFunc foreach_func) {
    HashMapIter* iter;
    void *       key, *value;

    if (list == NULL || foreach_func == NULL) {
        errno = EINVAL;
        return -1;
    }

    iter = hash_map_iter_create(list->process_map);
    while (hash_map_iter_next(iter, &key, &value) == 0) {
        ProcessEntry* process_entry;
        int           foreach_func_result;

        process_entry = value;
        if ((foreach_func_result = foreach_func(process_entry->process)) != 0) {
            return foreach_func_result;
        }
    }

    return 0;
}
