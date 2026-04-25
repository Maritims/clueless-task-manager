#include "metrics/ctm_process_metrics.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/local_lim.h>
#include <sys/stat.h>

#include "core/platform.h"
#include "internal/ctm_process_metrics_internal.h"

#define BUFFER_SIZE 1024
#define PROCESS_NAME_MAX 256

static int read_file(const char* file_path, char* buffer) {
    FILE*  fp;
    size_t bytes_read;

    if ((fp = fopen(file_path, "r")) == NULL) {
        return errno == ENOENT ? 1 : -1;
    }

    errno      = 0;
    bytes_read = fread(buffer, 1, BUFFER_SIZE - 1, fp);

    if (ferror(fp)) {
        errno = errno == 0 ? EIO : errno;
        fclose(fp);
        return -1;
    }

    buffer[bytes_read] = '\0';
    fclose(fp);
    return 0;
}

/* It is the responsibility of the caller to ensure that out_buffer is of an appropriate size, at least 32 byte */
static int get_pid_file_path(const unsigned int pid, char* out_buffer) {
    const int bytes_written = ctm_snprintf(out_buffer, BUFFER_SIZE, "/proc/%u/stat", pid);

    /* Encoding error or internal failure */
    if (bytes_written < 0) {
        fprintf(stderr, "get_pid_file_path: Failed to snprintf: %s\n", strerror(errno));
        errno = EINVAL;
        return -1;
    }
    /* The buffer was too small to hold the file path */
    if ((size_t) bytes_written >= BUFFER_SIZE) {
        errno = ENAMETOOLONG;
        return -1;
    }

    return 0;
}

/* It is the responsibility of the caller to ensure that out_buffer is of an appropriate size, at least 256 byte */
static int get_user_from_uid(const unsigned int uid, char* out_buffer) {
    int   bytes_written;
    char* local_buffer;

    if ((local_buffer = ctm_getusername_by_uid(uid)) == NULL) {
        return -1;
    }

    bytes_written = ctm_snprintf(out_buffer, BUFFER_SIZE, "%s", local_buffer);

    if (bytes_written < 0) {
        errno = EINVAL;
        return -1;
    }
    if ((size_t) bytes_written >= BUFFER_SIZE) {
        errno = ERANGE;
        return -1;
    }

    return 0;
}

static int get_process_name(char** out_end_ptr, char* out_buffer) {
    const char* opening;
    char*       closing;
    size_t      name_length;

    /* Find the opening parenthesis */
    opening = strchr(*out_end_ptr, '(');
    if (opening == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Find the closing parenthesis */
    closing = strrchr(opening, ')');
    if (closing == NULL || closing <= opening) {
        errno = EINVAL;
        return -1;
    }

    /* Calculate the length of the process name */
    name_length = (size_t) (closing - opening - 1);

    /* Prevent buffer overflow */
    if (name_length >= BUFFER_SIZE) {
        name_length = BUFFER_SIZE - 1;
    }

    memcpy(out_buffer, opening + 1, name_length);
    out_buffer[name_length] = '\0';

    *out_end_ptr = closing + 1;
    if (**out_end_ptr == ' ') {
        (*out_end_ptr)++;
    }

    return 0;
}

ctm_process_metrics_status_t ctm_process_metrics_read(unsigned int pid, ctm_process_metrics_t* out_value) {
    char        pid_file_path[PATH_MAX];
    char        pid_file_buffer[BUFFER_SIZE];
    char*       pid_file_buffer_pos; /* for moving through pid_file_buffer */
    struct stat file_info;           /* for retrieving uid */
    int         read_file_result;

    if (out_value == NULL) {
        return CTM_PROCESS_METRICS_ERR_INVALID_ARG;
    }

    if (get_pid_file_path(pid, pid_file_path) != 0) {
        return CTM_PROCESS_METRICS_ERR_NOT_FOUND;
    }

    if (stat(pid_file_path, &file_info) != 0) {
        if (errno == ENOENT) {
            /* Signal to the caller that the file does not exist. This is expected behaviour. */
            return CTM_PROCESS_METRICS_ERR_NOT_FOUND;
        }

        return CTM_PROCESS_METRICS_ERR_INTERNAL;
    }

    read_file_result = read_file(pid_file_path, pid_file_buffer);
    if (read_file_result != 0) {
        /**
         * @note READ_FILE_NOT_FOUND is considered an error in this context, because we expect the process to exist. The caller can consult errno to determine what the error was.
         */
        return CTM_PROCESS_METRICS_ERR_NOT_FOUND;
    }

    get_user_from_uid(file_info.st_uid, out_value->user);

    /* Assign to pid_file_buffer_pos after read_file but before get_process_name */
    pid_file_buffer_pos = pid_file_buffer;

    if (get_process_name(&pid_file_buffer_pos, out_value->name) != 0) {
        return CTM_PROCESS_METRICS_ERR_PARSE;
    }

    out_value->uid   = file_info.st_uid;
    out_value->state = '\0';

    /* Scan the file buffer from the pid_file_buffer_pos pointer which has been advanced past the process name by get_process_name */
    if (sscanf(pid_file_buffer_pos,
               " %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u "
               "%lu "                     /* Field 14: utime */
               "%lu "                     /* Field 15: stime */
               "%*d %*d %*d %*d %*d %*d " /* Field 16-21 */
               "%lu "                     /* Field 22: starttime */
               "%*u "                     /* Field 23: skipped */
               "%lu",                     /* Field 24: rss (Resident Set Size) */
               &out_value->state,
               &out_value->utime,
               &out_value->stime,
               &out_value->start_time,
               &out_value->rss) < 5) {
        fprintf(stderr, "process_get: Failed to parse %s: %d. File must contain at least 5 numbers\n", pid_file_path, errno);
        return CTM_PROCESS_METRICS_ERR_PARSE;
    }

    return CTM_PROCESS_METRICS_SUCCESS;
}

ctm_process_metrics_status_t ctm_process_metrics_destroy(ctm_process_metrics_t* process_metrics) {
    if (!process_metrics) {
        return CTM_PROCESS_METRICS_ERR_INVALID_ARG;
    }

    free(process_metrics);
    return CTM_PROCESS_METRICS_SUCCESS;
}

ctm_process_metrics_status_t ctm_process_metrics_get_pid(const ctm_process_metrics_t* process_metrics, unsigned int* out_value) {
    if (process_metrics == NULL || out_value == NULL) {
        return CTM_PROCESS_METRICS_ERR_INVALID_ARG;
    }

    *out_value = process_metrics->pid;
    return CTM_PROCESS_METRICS_SUCCESS;
}

ctm_process_metrics_status_t ctm_process_metrics_get_name(const ctm_process_metrics_t* process_metrics, char* out_buffer, size_t out_buffer_size) {
    if (process_metrics == NULL || out_buffer == NULL || out_buffer_size == 0) {
        return CTM_PROCESS_METRICS_ERR_INVALID_ARG;
    }

    ctm_snprintf(out_buffer, out_buffer_size, "%s", process_metrics->name);
    return CTM_PROCESS_METRICS_SUCCESS;
}

ctm_process_metrics_status_t ctm_process_metrics_get_state(const ctm_process_metrics_t* process_metrics, char* out_buffer, size_t out_buffer_size) {
    if (process_metrics == NULL || process_metrics->state == 0 || out_buffer == NULL || out_buffer_size == 0) {
        return CTM_PROCESS_METRICS_ERR_INVALID_ARG;
    }

    switch (process_metrics->state) {
        case 'R':
            ctm_snprintf(out_buffer, out_buffer_size, "Running");
            break;
        case 'S':
            ctm_snprintf(out_buffer, out_buffer_size, "Sleeping");
            break;
        case 'D':
            ctm_snprintf(out_buffer, out_buffer_size, "Disk Sleep");
            break;
        case 'Z':
            ctm_snprintf(out_buffer, out_buffer_size, "Zombie");
            break;
        case 'T':
            ctm_snprintf(out_buffer, out_buffer_size, "Traced/Stopped");
            break;
        case 'W':
            ctm_snprintf(out_buffer, out_buffer_size, "Paging/Waking");
            break;
        case 'X':
        case 'x':
            ctm_snprintf(out_buffer, out_buffer_size, "Dead");
            break;
        case 'K':
            ctm_snprintf(out_buffer, out_buffer_size, "Wake-kill");
            break;
        case 'P':
            ctm_snprintf(out_buffer, out_buffer_size, "Parked");
            break;
        case 'I':
            ctm_snprintf(out_buffer, out_buffer_size, "Idle");
            break;
        default:
            return CTM_PROCESS_METRICS_ERR_PARSE;
    }

    return CTM_PROCESS_METRICS_SUCCESS;
}

ctm_process_metrics_status_t ctm_process_metrics_get_user(const ctm_process_metrics_t* process_metrics, char* out_buffer, size_t out_buffer_size) {
    if (process_metrics == NULL || out_buffer == NULL || out_buffer_size == 0) {
        return CTM_PROCESS_METRICS_ERR_INVALID_ARG;
    }

    ctm_snprintf(out_buffer, out_buffer_size, "%s", process_metrics->user);
    return CTM_PROCESS_METRICS_SUCCESS;
}

ctm_process_metrics_status_t ctm_process_metrics_get_is_active(const ctm_process_metrics_t* process_metrics, int* out_value) {
    if (process_metrics == NULL || out_value == NULL) {
        return CTM_PROCESS_METRICS_ERR_INVALID_ARG;
    }

    *out_value = process_metrics->is_active;
    return CTM_PROCESS_METRICS_SUCCESS;
}

ctm_process_metrics_status_t ctm_process_metrics_set_is_active(ctm_process_metrics_t* process_metrics, const int is_active) {
    if (process_metrics == NULL) {
        return CTM_PROCESS_METRICS_ERR_INVALID_ARG;
    }

    process_metrics->is_active = is_active;
    return CTM_PROCESS_METRICS_SUCCESS;
}

const char* ctm_process_metrics_strerror(const ctm_process_metrics_status_t process_metrics_status) {
    switch (process_metrics_status) {
        case CTM_PROCESS_METRICS_SUCCESS:
            return "Success";
        case CTM_PROCESS_METRICS_ERR_INVALID_ARG:
            return "Invalid argument";
        case CTM_PROCESS_METRICS_ERR_NOT_FOUND:
            return "Process not found";
        case CTM_PROCESS_METRICS_ERR_INTERNAL:
            return "Internal error";
        case CTM_PROCESS_METRICS_ERR_OPEN:
            return "Failed to open file";
        case CTM_PROCESS_METRICS_ERR_READ:
            return "Failed to read file";
        case CTM_PROCESS_METRICS_ERR_PARSE:
            return "Failed to parse file";
        default:
            return "Unknown error";
    }
}
