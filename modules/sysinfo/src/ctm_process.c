//
// Created by martin on 27.03.2026.
//

#include "ctm_process.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define CTM_PROCESS_USERNAME_LENGTH 256
#define CTM_PROCESS_NAME_LENGTH 256

struct CtmProcess {
    pid_t         pid;
    unsigned int  uid;
    char          username[CTM_PROCESS_USERNAME_LENGTH];
    char          state;
    char          name[CTM_PROCESS_NAME_LENGTH];
    unsigned long utime;  // user time ticks
    unsigned long stime;  // kernel time ticks
    unsigned long rss_kb; // resident set size, the portion of memory (measured in kilobytes) occupied by a process that is held in RAM.
};

struct CtmProcessArray {
    CtmProcess* elements;
    size_t      count;
    size_t      capacity;
};

static int is_pid_dir(const char* name) {
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

CtmProcessArray* ctm_process_array_new(void) {
    CtmProcessArray* array = malloc(sizeof(CtmProcessArray));
    if (!array) {
        fprintf(stderr, "Failed to allocate memory for process array: %d\n", errno);
        return NULL;
    }

    array->elements = NULL;
    array->count    = 0;
    array->capacity = 0;

    return array;
}

void ctm_process_array_free(CtmProcessArray* array) {
    if (array) {
        free(array->elements);
        free(array);
    }
}

CtmProcess* ctm_process_new(const pid_t pid, const unsigned int uid, const char* name, const char* state, const char* username, const unsigned long utime, const unsigned long stime, const unsigned long rss_kb) {
    CtmProcess* process = malloc(sizeof(CtmProcess));
    if (!process) {
        fprintf(stderr, "Failed to allocate memory for process: %d\n", errno);
        return NULL;
    }

    process->pid = pid;
    process->uid = uid;
    strncpy(process->username, username, sizeof(process->username));
    strncpy(process->name, name, sizeof(process->name));
    process->state  = state[0];
    process->utime  = utime;
    process->stime  = stime;
    process->rss_kb = rss_kb;

    return process;
}

void ctm_process_free(CtmProcess* process) {
    if (process) {
        free(process);
    }
}

CtmProcess* ctm_process_from_kernel(const pid_t pid, const int include_all_users) {
    char        path[64];
    struct stat file_info;
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    if (stat(path, &file_info) != 0) {
        fprintf(stderr, "%s: Failed to stat %s: %s\n", __func__, path, strerror(errno));
        return NULL;
    }

    if (!include_all_users && file_info.st_uid != getuid()) {
        // The process belongs to someone else.
        return NULL;
    }

    FILE* fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "%s: Failed to open %s: %s\n", __func__, path, strerror(errno));
        return NULL;
    }
    char         buffer[1024];
    const size_t bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    if (bytes_read == 0) {
        fprintf(stderr, "%s: Failed to read %s: %d\n", __func__, path, errno);
        return NULL;
    }
    buffer[bytes_read] = '\0';

    const char* opening_parenthesis = strchr(buffer, '(');
    const char* closing_parenthesis = strrchr(opening_parenthesis, ')');
    if (!opening_parenthesis || !closing_parenthesis || opening_parenthesis > closing_parenthesis) {
        fprintf(stderr, "%s: Failed to parse %s: %d. File must contain an opening parenthesis followed by a closing parenthesis\n", __func__, path, errno);
        return NULL;
    }

    const unsigned int uid = file_info.st_uid;
    char               name[CTM_PROCESS_NAME_LENGTH];
    char               state[16];
    char               username[CTM_PROCESS_USERNAME_LENGTH];
    unsigned long      utime;
    unsigned long      stime;
    unsigned long      rss_kb;

    const struct passwd* pw = getpwuid(uid);
    strncpy(username, pw != NULL ? pw->pw_name : "Unknown", sizeof(username));

    // Ensure name_length doesn't exceed the available space in name.
    size_t name_length = closing_parenthesis - opening_parenthesis - 1;
    if (name_length > CTM_PROCESS_NAME_LENGTH - 1) {
        name_length = CTM_PROCESS_NAME_LENGTH - 1;
    }
    strncpy(name, opening_parenthesis + 1, name_length);
    name[name_length] = '\0';

    // Jump past the closing parenthesis before starting to read numbers.
    closing_parenthesis += 2;

    const int scanned = sscanf(closing_parenthesis,
                               "%c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu " // Fields 3-13 (Skipped)
                               "%lu "                                            // Field 14: utime
                               "%lu "                                            // Field 15: stime
                               "%*ld %*ld %*ld %*ld %*ld %*ld %*llu %*lu "       // Fields 16-23 (Skipped)
                               "%ld",                                            // Field 24: rss (Resident Set Size)
                               state,
                               &utime,
                               &stime,
                               &rss_kb);

    if (scanned < 3) {
        fprintf(stderr, "%s: Failed to parse %s: %d. File must contain at least 3 numbers\n", __func__, path, errno);
        return NULL;
    }

    return ctm_process_new(pid, uid, name, state, username, utime, stime, rss_kb);
}

CtmProcessArray* ctm_processes_from_kernel(const int include_all_users) {
    CtmProcessArray* array = ctm_process_array_new();
    if (!array) {
        fprintf(stderr, "Failed to create process array: %s\n", strerror(errno));
        return NULL;
    }

    DIR* dir = opendir("/proc");
    if (!dir) {
        fprintf(stderr, "Failed to open /proc: %s\n", strerror(errno));
        ctm_process_array_free(array);
        return NULL;
    }

    array->count = 0;
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_DIR || is_pid_dir(entry->d_name) != 0) {
            // Either not a directory at all or not a pid directory.
            continue;
        }

        const int         pid     = (int) strtol(entry->d_name, NULL, 10);
        const CtmProcess* process = ctm_process_from_kernel(pid, include_all_users);
        if (process == NULL) {
            continue;
        }

        const int push_result = ctm_process_array_push(array, process);
        if (push_result != 0) {
            fprintf(stderr, "Failed to push process to array: %s\n", strerror(push_result));
        }
    }

    closedir(dir);

    return array;
}

size_t ctm_process_array_get_count(const CtmProcessArray* array) {
    if (array == NULL) {
        fprintf(stderr, "ctm_process_array_get_count: array cannot be NULL\n");
        return 0;
    }
    return array->count;
}

size_t ctm_process_array_get_capacity(const CtmProcessArray* array) {
    if (array == NULL) {
        fprintf(stderr, "ctm_process_array_get_capacity: array cannot be NULL\n");
        return 0;
    }
    return array->capacity;
}

const CtmProcess* ctm_process_array_get_element(const CtmProcessArray* array, const size_t index) {
    if (array == NULL) {
        fprintf(stderr, "ctm_process_array_get_element: array cannot be NULL\n");
        return NULL;
    }
    if (index >= array->count) {
        fprintf(stderr, "ctm_process_array_get_element: index %zu is out of bounds\n", index);
        return NULL;
    }
    return &array->elements[index];
}

const CtmProcess* ctm_process_array_get_elements(const CtmProcessArray* array) {
    if (array == NULL) {
        fprintf(stderr, "ctm_process_array_get_elements: array cannot be NULL\n");
        return NULL;
    }
    return array->elements;
}

pid_t ctm_process_get_pid(const CtmProcess* process) {
    if (process == NULL) {
        fprintf(stderr, "ctm_process_get_pid: process cannot be NULL\n");
        return 0;
    }
    return process->pid;
}

const char* ctm_process_get_name(const CtmProcess* process) {
    if (process == NULL) {
        fprintf(stderr, "ctm_process_get_name: process cannot be NULL\n");
        return NULL;
    }
    return process->name;
}

const char* ctm_process_get_username(const CtmProcess* process) {
    if (process == NULL) {
        fprintf(stderr, "ctm_process_get_username: process cannot be NULL\n");
        return NULL;
    }
    return process->username;
}

const char* ctm_process_get_state(const CtmProcess* process) {
    if (process == NULL) {
        fprintf(stderr, "ctm_process_get_state: process cannot be NULL\n");
        return NULL;
    }
    if (process->pid == 0) {
        fprintf(stderr, "ctm_process_get_state: process->pid cannot be 0\n");
        return NULL;
    }

    switch (process->state) {
        case 'R':
            return "Running";
        case 'S':
            return "Sleeping";
        case 'D':
            return "Disk Sleep";
        case 'Z':
            return "Zombie";
        case 'T':
            return "Traced/Stopped";
        case 'W':
            return "Paging/Waking";
        case 'X':
        case 'x':
            return "Dead";
        case 'K':
            return "Wake-kill";
        case 'P':
            return "Parked";
        case 'I':
            return "Idle";
        default:
            fprintf(stderr, "ctm_process_translate_state: Unknown state for pid %d: %c\n", process->pid, process->state);
            return NULL;
    }
}

int ctm_process_array_push(CtmProcessArray* array, const CtmProcess* item) {
    if (!array || !item) {
        fprintf(stderr, "process_array_push: Invalid argument: NULL\n");
        return errno = EINVAL;
    }

    if (array->count >= array->capacity) {
        const size_t new_capacity = array->capacity == 0 ? 100 : array->capacity * 2;
        CtmProcess*  new_ptr      = realloc(array->elements, new_capacity * sizeof(CtmProcess));
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
