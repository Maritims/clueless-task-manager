//
// Created by martin on 27.03.2026.
//

#include "ctm_process.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>

static int is_pid_dir(const char *name) {
    if (!name) {
        fprintf(stderr, "is_pid_dir: name: NULL\n");
        return EINVAL;
    }

    for (unsigned long i = 0; i < strlen(name); i++) {
        if (!isdigit(name[i])) {
            return EINVAL;
        }
    }

    return 0;
}

static int parse_proc_stat(const int pid, CtmProcess *process, const int include_all_users) {
    if (!pid) {
        fprintf(stderr, "parse_proc_stat: pid: %d\n", pid);
        return EINVAL;
    }

    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);

    struct stat file_info;
    if (stat(path, &file_info) != 0) {
        fprintf(stderr, "Failed to stat %s: %s\n", path, strerror(errno));
        return errno;
    }

    if (!include_all_users && file_info.st_uid != getuid()) {
        // The process belongs to someone else.
        return 0;
    }

    process->uid = file_info.st_uid;

    const struct passwd *pw = getpwuid(process->uid);
    if (pw != NULL) {
        strncpy(process->username, pw->pw_name, sizeof(process->username));
    } else {
        strncpy(process->username, "Unknown", sizeof(process->username));
    }

    FILE *fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "Failed to open %s: %s\n", path, strerror(errno));
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

    if (process->pid == 0) {
        fprintf(stderr, "Failed to parse %s: %d. File must contain a valid PID\n", path, errno);
        return errno;
    }

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
                               "%c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu " // Fields 3-13 (Skipped)
                               "%lu "                                            // Field 14: utime
                               "%lu "                                            // Field 15: stime
                               "%*ld %*ld %*ld %*ld %*ld %*ld %*llu %*lu "       // Fields 16-23 (Skipped)
                               "%ld",                                            // Field 24: rss (Resident Set Size)
                               &process->state,
                               &process->utime,
                               &process->stime,
                               &process->rss_kb);

    if (scanned < 3) {
        fprintf(stderr, "Failed to parse %s: %d. File must contain at least 3 numbers\n", path, errno);
        return errno;
    }

    return 0;
}

int ctm_process_array_init(CtmProcessArray *array) {
    if (!array) {
        fprintf(stderr, "ctm_process_array_init: Invalid argument: NULL\n");
        return errno = EINVAL;
    }

    array->elements = NULL;
    array->count    = 0;
    array->capacity = 0;

    return 0;
}

int ctm_process_array_destroy(CtmProcessArray *array) {
    if (!array) {
        fprintf(stderr, "ctm_process_array_destroy: Invalid argument: NULL\n");
        return errno = EINVAL;
    }

    free(array->elements);
    *array = (CtmProcessArray){0};

    return 0;
}

int process_array_push(CtmProcessArray *array, const CtmProcess *item) {
    if (!array || !item) {
        fprintf(stderr, "process_array_push: Invalid argument: NULL\n");
        return errno = EINVAL;
    }

    if (array->count >= array->capacity) {
        const size_t new_capacity = array->capacity == 0 ? 100 : array->capacity * 2;
        CtmProcess * new_ptr      = realloc(array->elements, new_capacity * sizeof(CtmProcess));
        if (!new_ptr) {
            fprintf(stderr, "Failed to allocate memory for process array: %d\n", errno);
            return errno;
        }
        array->elements = new_ptr;
        array->capacity = new_capacity;
    }

    array->elements[array->count++] = *item;

    return 0;
}

int ctm_process_fetch_all(CtmProcessArray *array, int include_all_users) {
    if (!array) {
        fprintf(stderr, "process_fetch_all: Invalid argument: NULL\n");
        return errno = EINVAL;
    }

    DIR *dir = opendir("/proc");
    if (!dir) {
        return errno = EIO;
    }

    array->count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_DIR || is_pid_dir(entry->d_name) != 0) {
            // Either not a directory at all or not a pid directory.
            continue;
        }

        const int  pid          = (int) strtol(entry->d_name, NULL, 10);
        CtmProcess process      = {0};
        const int  parse_result = parse_proc_stat(pid, &process, include_all_users);

        if (parse_result > 0) {
            fprintf(stderr, "Failed to parse /proc/%d/stat: %s\n", pid, strerror(parse_result));
            closedir(dir);
            return parse_result;
        }

        if (parse_result == -1) {
            // Does not belong to the current user.
            continue;
        }

        const int push_result = process_array_push(array, &process);
        if (push_result != 0) {
            fprintf(stderr, "Failed to push process to array: %s\n", strerror(push_result));
            closedir(dir);
            return push_result;
        }
    }

    closedir(dir);

    return 0;
}

int ctm_process_translate_state(const CtmProcess *process, char *out_state) {
    if (process == NULL) {
        fprintf(stderr, "ctm_process_translate_state: process cannot be NULL\n");
        return errno = EINVAL;
    }
    if (process->pid == 0) {
        fprintf(stderr, "ctm_process_translate_state: process->pid cannot be 0\n");
        return errno = EINVAL;
    }
    if (out_state == NULL) {
        fprintf(stderr, "ctm_process_translate_state: out_state cannot be NULL\n");
        return errno = EINVAL;
    }

    switch (process->state) {
        case 'R':
            strcpy(out_state, "Running");
            break;
        case 'S':
            strcpy(out_state, "Sleeping");
            break;
        case 'D':
            strcpy(out_state, "Disk Sleep");
            break;
        case 'Z':
            strcpy(out_state, "Zombie");
            break;
        case 'T':
            strcpy(out_state, "Traced/Stopped");
            break;
        case 'W':
            strcpy(out_state, "Paging/Waking");
            break;
        case 'X':
        case 'x':
            strcpy(out_state, "Dead");
            break;
        case 'K':
            strcpy(out_state, "Wake-kill");
            break;
        case 'P':
            strcpy(out_state, "Parked");
            break;
        case 'I':
            strcpy(out_state, "Idle");
            break;
        default:
            fprintf(stderr, "ctm_process_translate_state: Unknown state for pid %d: %c\n", process->pid, process->state);
            return -1;
    }

    return 0;
}
