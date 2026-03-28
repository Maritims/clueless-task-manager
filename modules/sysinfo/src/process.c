//
// Created by martin on 27.03.2026.
//

#include "process.h"

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Checks whether every character in the given name is a digit.
 */
static int is_pid_dir(const char *name) {
    if (!name) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return 0;
    }

    for (int i = 0; i < strlen(name); i++) {
        if (!isdigit(name[i])) return 0;
    }
    return 1;
}

stat_result_t process_fetch_all(process_array_t *array) {
    if (!array) {
        return STAT_ERR_INVALID;
    }

    DIR *dir = opendir("/proc");
    if (!dir) {
        return STAT_ERR_OPEN;
    }

    array->count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && is_pid_dir(entry->d_name)) {
            // Check array capacity and grow if needed.
            if (array->count >= array->capacity) {
                const size_t new_capacity = array->capacity == 0 ? 32 : array->capacity * 2;
                process_t *  new_ptr      = realloc(array->processes, new_capacity * sizeof(process_t));
                if (!new_ptr) {
                    closedir(dir);
                    return STAT_ERR_MALLOC;
                }
                array->processes = new_ptr;
                array->capacity  = new_capacity;
            }

            process_t *process = &array->processes[array->count++];
            char *     end     = NULL;
            process->pid       = (int) strtol(entry->d_name, &end, 10);
            snprintf(process->name, sizeof(process->name), "%s", entry->d_name);

            char path[512];
            snprintf(path, sizeof(path), "/proc/%d/stat", process->pid);
            FILE *fp = fopen(path, "r");
            if (!fp) {
                closedir(dir);
                return STAT_ERR_OPEN;
            }

            int scanned = fscanf(fp,
                                 "%*d (%255[^)]) %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
                                 process->name,
                                 &process->utime,
                                 &process->stime);

            fclose(fp);
        }

        return STAT_SUCCESS;
    }

    return STAT_ERR_INVALID;
}

void process_array_free(process_array_t *array) {
    if (array) {
        free(array->processes);
        array->processes = NULL;
        array->count     = 0;
        array->capacity  = 0;
    }
}
