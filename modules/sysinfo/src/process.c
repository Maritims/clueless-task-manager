//
// Created by martin on 27.03.2026.
//

#include "process.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_pid_dir(const char *name) {
    if (!name) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return 0;
    }

    for (unsigned long i = 0; i < strlen(name); i++) {
        if (!isdigit(name[i])) return 0;
    }
    return 1;
}

static int parse_proc_stat(const pid_t pid, process_t *process) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Failed to open %s: %d\n", path, errno);
        return errno;
    }

    char         buffer[1024];
    const size_t bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    if (bytes_read == 0) {
        fprintf(stderr, "Failed to read %s: %d\n", path, errno);
        return errno;
    }
    buffer[bytes_read] = '\0';

    char *end_ptr;
    process->pid = (int) strtol(buffer, &end_ptr, 10);

    const char *opening_parenthesis = strchr(end_ptr, '(');
    const char *closing_parenthesis = strrchr(opening_parenthesis, ')');
    if (!opening_parenthesis || !closing_parenthesis || opening_parenthesis > closing_parenthesis) {
        fprintf(stderr, "Failed to parse %s: %d. File must contain an opening parenthesis followed by a closing parenthesis\n", path, errno);
        return errno;
    }

    // Ensure name_length doesn't exceed the available space in process->name.
    size_t name_length = closing_parenthesis - opening_parenthesis - 1;
    if (name_length > sizeof(process->name) - 1) {
        name_length = sizeof(process->name) - 1;
    }
    strncpy(process->name, opening_parenthesis + 1, name_length);
    process->name[name_length] = '\0';

    // Jump past the closing parenthesis before starting to read numbers.
    closing_parenthesis += 2;

    const int scanned = sscanf(closing_parenthesis,
                         "%*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu " // Fields 3-13 (Skipped)
                         "%lu "                                             // Field 14: utime
                         "%lu "                                             // Field 15: stime
                         "%*ld %*ld %*ld %*ld %*ld %*ld %*llu %*lu "        // Fields 16-23 (Skipped)
                         "%ld",                                             // Field 24: rss (Resident Set Size)
                         &process->utime,
                         &process->stime,
                         &process->rss_kb);

    if (scanned < 3) {
        fprintf(stderr, "Failed to parse %s: %d. File must contain at least 3 numbers\n", path, errno);
        return errno;
    }

    return 0;
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

            process_t *process = &array->processes[array->count];
            const int pid = (int) strtol(entry->d_name, NULL, 10);
            if (parse_proc_stat(pid, process) == 0) {
                array->count++;
            }
        }
    }

    return STAT_SUCCESS;
}

void process_array_free(process_array_t *array) {
    if (array) {
        free(array->processes);
        array->processes = NULL;
        array->count     = 0;
        array->capacity  = 0;
    }
}
