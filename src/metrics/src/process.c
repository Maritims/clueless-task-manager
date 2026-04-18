#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/local_lim.h>
#include <sys/stat.h>

#include "metrics/process.h"

#define PROCESS_NAME_MAX 16

struct Process {
    unsigned int  pid;
    unsigned int  uid;
    char          username[LOGIN_NAME_MAX];
    char          state;
    char          name[PROCESS_NAME_MAX];
    unsigned long utime; /* user time ticks */
    unsigned long stime; /* kernel time ticks */
    long          rss;   /* resident set size, the portion of memory (measured in kilobytes) occupied by a process that is held in RAM. */
    unsigned long start_time;
};

typedef enum {
    READ_FILE_OK,
    READ_FILE_ERROR,
    READ_FILE_NOT_FOUND
} ReadFileResult;

static ReadFileResult read_file(const char* file_path, char* buffer, const size_t buffer_size) {
    FILE*  fp;
    size_t bytes_read;

    if ((fp = fopen(file_path, "r")) == NULL) {
        return errno == ENOENT ? READ_FILE_NOT_FOUND : READ_FILE_ERROR;
    }

    errno      = 0;
    bytes_read = fread(buffer, 1, buffer_size - 1, fp);

    if (ferror(fp)) {
        errno = errno == 0 ? EIO : errno;
        fclose(fp);
        return READ_FILE_ERROR;
    }

    buffer[bytes_read] = '\0';
    fclose(fp);
    return READ_FILE_OK;
}

static int get_pid_file_path(const unsigned int pid, char* out_buffer, const size_t out_buffer_size) {
    int bytes_written;

    bytes_written = snprintf(out_buffer, out_buffer_size, "/proc/%u/stat", pid);

    /* Encoding error or internal failure */
    if (bytes_written < 0) {
        fprintf(stderr, "get_pid_file_path: Failed to snprintf: %s\n", strerror(errno));
        errno = EINVAL;
        return -1;
    }
    /* The buffer was too small to hold the file path */
    if ((size_t) bytes_written >= out_buffer_size) {
        errno = ENAMETOOLONG;
        return -1;
    }

    return 0;
}

static int get_user_from_uid(const uid_t uid, char* out_buffer, const size_t out_buffer_size) {
    int            bytes_written;
    struct passwd  pwd;
    struct passwd* result;
    char           local_buffer[1024];
    int            pwuid_result_code;

    pwuid_result_code = getpwuid_r(uid, &pwd, local_buffer, sizeof(local_buffer), &result);
    if (pwuid_result_code != 0) {
        errno = pwuid_result_code;
        return -1;
    }

    if (result == NULL) {
        return 0;
    }

    bytes_written = snprintf(out_buffer, out_buffer_size, "%s", pwd.pw_name);

    if (bytes_written < 0) {
        errno = EINVAL;
        return -1;
    }
    if ((size_t) bytes_written >= out_buffer_size) {
        errno = ERANGE;
        return -1;
    }

    return 0;
}

static int get_process_name(char** out_end_ptr, char* out_buffer, const size_t out_buffer_size) {
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
    if (name_length >= out_buffer_size) {
        name_length = out_buffer_size - 1;
    }

    memcpy(out_buffer, opening + 1, name_length);
    out_buffer[name_length] = '\0';

    *out_end_ptr = closing + 1;
    if (**out_end_ptr == ' ') {
        (*out_end_ptr)++;
    }

    return 0;
}

ProcessCaptureResult process_capture(Process* process) {
    char           pid_file_path[PATH_MAX];
    char           pid_file_buffer[1024];
    char*          pid_file_buffer_pos; /* for moving through pid_file_buffer */
    struct stat    file_info;           /* for retrieving uid */
    ReadFileResult read_file_result;

    if (process == NULL) {
        fprintf(stderr, "process_capture: process cannot be NULL\n");
        errno = EINVAL;
        return PROCESS_CAPTURE_ERROR;
    }

    if (get_pid_file_path(process->pid, pid_file_path, sizeof(pid_file_path)) != 0) {
        fprintf(stderr, "process_capture: Failed to get pid file path for pid %u: %s\n", process->pid, strerror(errno));
        return PROCESS_CAPTURE_ERROR;
    }

    if (stat(pid_file_path, &file_info) != 0) {
        if (errno == ENOENT) {
            /* Signal to the caller that the file does not exist. This is expected behaviour. */
            return PROCESS_CAPTURE_NOT_FOUND;
        }

        fprintf(stderr, "process_capture: Failed to stat %s: %s\n", pid_file_path, strerror(errno));
        return PROCESS_CAPTURE_NOT_FOUND;
    }
    
    read_file_result = read_file(pid_file_path, pid_file_buffer, sizeof(pid_file_buffer));
    if (read_file_result == READ_FILE_NOT_FOUND || read_file_result == READ_FILE_ERROR) {
        /**
         * @note READ_FILE_NOT_FOUND is considered an error in this context, because we expect the process to exist. The caller can consult errno to determine what the error was.
         */
        return PROCESS_CAPTURE_ERROR;
    }

    get_user_from_uid(file_info.st_uid, process->username, sizeof(process->username));

    /* Assign to pid_file_buffer_pos after read_file but before get_process_name */
    pid_file_buffer_pos = pid_file_buffer;

    if (get_process_name(&pid_file_buffer_pos, process->name, sizeof(process->name)) != 0) {
        fprintf(stderr, "process_capture: Failed to get process name for pid %u: %s\n", process->pid, strerror(errno));
        return PROCESS_CAPTURE_ERROR;
    }

    process->pid   = process->pid;
    process->uid   = file_info.st_uid;
    process->state = '\0';

    /* Scan the file buffer from the pid_file_buffer_pos pointer which has been advanced past the process name by get_process_name */
    if (sscanf(pid_file_buffer_pos,
               /* Fields 3-13 (Skipped) */
               " %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u "
               "%lu "                     /* Field 14: utime */
               "%lu "                     /* Field 15: stime */
               "%*d %*d %*d %*d %*d %*d " /* Field 16-21 */
               "%lu "                     /* Field 22: starttime */
               "%*u "                     /* Field 23: skipped */
               "%ld",                     /* Field 24: rss (Resident Set Size) */
               &process->state,
               &process->utime,
               &process->stime,
               &process->start_time,
               &process->rss) < 5) {
        fprintf(stderr, "process_get: Failed to parse %s: %d. File must contain at least 5 numbers\n", pid_file_path, errno);
        return PROCESS_CAPTURE_ERROR;
    }

    return PROCESS_CAPTURE_OK;
}

Process* process_get(const unsigned int pid) {
    Process*             process;
    ProcessCaptureResult process_capture_result;

    process = malloc(sizeof(Process));
    if (process == NULL) {
        return NULL;
    }

    process->pid           = pid;
    process_capture_result = process_capture(process);

    if (process_capture_result == PROCESS_CAPTURE_OK) {
        return process;
    }
    if (process_capture_result == PROCESS_CAPTURE_NOT_FOUND) {
        process_free(process);
        /**
         * @note We set errno so the caller can understand the difference between the two cases which return NULL.
         */
        errno = ENOENT;
        return NULL;
    }

    fprintf(stderr, "process_get: Failed to capture process %u: %s\n", pid, strerror(errno));
    process_free(process);
    return NULL;
}

void process_free(Process* process) {
    if (process) {
        free(process);
    }
}

unsigned int process_get_pid(const Process* process) {
    if (process == NULL) {
        fprintf(stderr, "ctm_proc_pid_stat_get_pid: process cannot be NULL\n");
        return 0;
    }
    return process->pid;
}

const char* process_get_name(const Process* process) {
    if (process == NULL) {
        fprintf(stderr, "ctm_proc_pid_stat_get_name: process cannot be NULL\n");
        return NULL;
    }
    return process->name;
}

const char* process_get_state(const Process* process) {
    if (process == NULL) {
        fprintf(stderr, "ctm_proc_pid_stat_get_state: proc_pid_stat cannot be NULL\n");
        return NULL;
    }
    if (process->pid == 0) {
        fprintf(stderr, "ctm_proc_pid_stat_get_state: proc_pid_stat->pid cannot be 0\n");
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

const char* process_get_user(const Process* process) {
    if (process == NULL) {
        fprintf(stderr, "process_get_user: proc_pid_stat cannot be NULL\n");
        return NULL;
    }
    return process->username;
}
