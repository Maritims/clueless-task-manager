#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/local_lim.h>
#include <sys/stat.h>

#include "process.h"
#include "platform.h"
#include "internal/process_internal.h"

#define BUFFER_SIZE 1024
#define PROCESS_NAME_MAX 256

static int read_file(const char* file_path,
                     char*       buffer)
{
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
static int get_pid_file_path(const unsigned int pid,
                             char*              out_buffer)
{
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
static int get_user_from_uid(const unsigned int uid,
                             char*              out_buffer)
{
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

static int get_process_name(char** out_end_ptr,
                            char*  out_buffer)
{
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

const char* process_strerror(const process_result_t result)
{
    switch (result) {
        case PROCESS_SUCCESS:
            return "Success";
        case PROCESS_ERR_INVALID_ARG:
            return "Invalid argument";
        case PROCESS_ERR_NOT_FOUND:
            return "Process not found";
        case PROCESS_ERR_INTERNAL:
            return "Internal error";
        case PROCESS_ERR_OPEN:
            return "Failed to open file";
        case PROCESS_ERR_READ:
            return "Failed to read file";
        case PROCESS_ERR_PARSE:
            return "Failed to parse file";
        default:
            return "Unknown error";
    }
}

process_result_t process_read(const unsigned int pid,
                              process_t*         out)
{
    char        pid_file_path[PATH_MAX];
    char        pid_file_buffer[BUFFER_SIZE];
    char*       pid_file_buffer_pos; /* for moving through pid_file_buffer */
    struct stat file_info;           /* for retrieving uid */
    int         read_file_result;

    if (out == NULL) {
        return PROCESS_ERR_INVALID_ARG;
    }

    if (get_pid_file_path(pid, pid_file_path) != 0) {
        return PROCESS_ERR_NOT_FOUND;
    }

    if (stat(pid_file_path, &file_info) != 0) {
        if (errno == ENOENT) {
            /* Signal to the caller that the file does not exist. This is expected behaviour. */
            return PROCESS_ERR_NOT_FOUND;
        }

        return PROCESS_ERR_INTERNAL;
    }

    read_file_result = read_file(pid_file_path, pid_file_buffer);
    if (read_file_result != 0) {
        /**
         * @note READ_FILE_NOT_FOUND is considered an error in this context, because we expect the process to exist. The caller can consult errno to determine what the error was.
         */
        return PROCESS_ERR_NOT_FOUND;
    }

    get_user_from_uid(file_info.st_uid, out->user);

    /* Assign to pid_file_buffer_pos after read_file but before get_process_name */
    pid_file_buffer_pos = pid_file_buffer;

    if (get_process_name(&pid_file_buffer_pos, out->name) != 0) {
        return PROCESS_ERR_PARSE;
    }

    out->uid   = file_info.st_uid;
    out->state = '\0';

    /* Scan the file buffer from the pid_file_buffer_pos pointer which has been advanced past the process name by get_process_name */
    if (sscanf(pid_file_buffer_pos,
               " %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u "
               "%lu "                     /* Field 14: utime */
               "%lu "                     /* Field 15: stime */
               "%*d %*d %*d %*d %*d %*d " /* Field 16-21 */
               "%lu "                     /* Field 22: starttime */
               "%*u "                     /* Field 23: skipped */
               "%lu",                     /* Field 24: rss (Resident Set Size) */
               &out->state,
               &out->utime,
               &out->stime,
               &out->start_time,
               &out->rss) < 5) {
        fprintf(stderr, "process_get: Failed to parse %s: %d. File must contain at least 5 numbers\n", pid_file_path, errno);
        return PROCESS_ERR_PARSE;
    }

    return PROCESS_SUCCESS;
}

process_result_t process_destroy(process_t* process)
{
    if (process) {
        free(process);
        return PROCESS_SUCCESS;
    }
    return PROCESS_ERR_INVALID_ARG;
}

process_result_t process_sizeof(size_t* out)
{
    if (out) {
        *out = sizeof(process_t);
        return PROCESS_SUCCESS;
    }
    return PROCESS_ERR_INVALID_ARG;
}

process_result_t process_from_node(list_node_t* node, process_t** out)
{
    if (node && out) {
        *out = LIST_ENTRY(node, process_t, node);
        return PROCESS_SUCCESS;
    }
    return PROCESS_ERR_INVALID_ARG;
}

process_result_t process_pid(const process_t* process,
                             unsigned int*    out)
{
    if (process && out) {
        *out = process->pid;
        return PROCESS_SUCCESS;
    }
    return PROCESS_ERR_INVALID_ARG;
}

process_result_t process_name(const process_t* process,
                              char*            out,
                              size_t           len)
{
    if (process && out && len) {
        ctm_snprintf(out, len, "%s", process->name);
        return PROCESS_SUCCESS;
    }
    return PROCESS_ERR_INVALID_ARG;
}

process_result_t process_state(const process_t* process,
                               char*            out,
                               const size_t     len)
{
    if (process && out && len) {
        switch (process->state) {
            case 'R':
                ctm_snprintf(out, len, "Running");
                break;
            case 'S':
                ctm_snprintf(out, len, "Sleeping");
                break;
            case 'D':
                ctm_snprintf(out, len, "Disk Sleep");
                break;
            case 'Z':
                ctm_snprintf(out, len, "Zombie");
                break;
            case 'T':
                ctm_snprintf(out, len, "Traced/Stopped");
                break;
            case 'W':
                ctm_snprintf(out, len, "Paging/Waking");
                break;
            case 'X':
            case 'x':
                ctm_snprintf(out, len, "Dead");
                break;
            case 'K':
                ctm_snprintf(out, len, "Wake-kill");
                break;
            case 'P':
                ctm_snprintf(out, len, "Parked");
                break;
            case 'I':
                ctm_snprintf(out, len, "Idle");
                break;
            default:
                return PROCESS_ERR_INVALID_ARG;
        }
        return PROCESS_SUCCESS;
    }
    return PROCESS_ERR_INVALID_ARG;
}

process_result_t process_user(const process_t* process,
                              char*            out,
                              const size_t     len)
{
    if (process && out && len) {
        ctm_snprintf(out, len, "%s", process->user);
        return PROCESS_SUCCESS;
    }
    return PROCESS_ERR_INVALID_ARG;
}

process_result_t process_active(const process_t* process,
                                int*             out)
{
    if (process && out) {
        *out = process->is_active;
        return PROCESS_SUCCESS;
    }
    return PROCESS_ERR_INVALID_ARG;
}

process_result_t process_activate(process_t* process)
{
    if (process) {
        process->is_active = 1;
        return PROCESS_SUCCESS;
    }
    return PROCESS_ERR_INVALID_ARG;
}

process_result_t process_deactivate(process_t* process)
{
    if (process) {
        process->is_active = 0;
        return PROCESS_SUCCESS;
    }
    return PROCESS_ERR_INVALID_ARG;
}
