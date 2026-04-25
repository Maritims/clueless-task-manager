#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#include <errno.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#include "core/ctm_platform.h"

int ctm_snprintf(char* buffer, size_t size, const char* format, ...) {
    int result;

    va_list args;
    va_start(args, format);
    /* On Linux, this is provided by the POSIX headers we unlocked above */
    result = vsnprintf(buffer, size, format, args);
    va_end(args);
    return result;
}

char* ctm_strtok_r(char* str, const char* delim, char** saveptr) {
    return strtok_r(str, delim, saveptr);
}

char* ctm_getusername_by_uid(unsigned int uid) {
    struct passwd  pwd;
    struct passwd* result;
    char*          username = NULL;
    size_t         bufsize;
    char*          buf;
    int            s;

    bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == (size_t) -1) {
        bufsize = 16384;
    }

    buf = malloc(bufsize);
    if (!buf) {
        return NULL;
    }

    s = getpwuid_r(uid, &pwd, buf, bufsize, &result);
    if (result && s == 0) {
        /* We strdup so the caller doesn't have to deal with our temp buffer */
        username = strdup(pwd.pw_name);
    }

    free(buf);
    return username;
}

void ctm_sleep_ms(unsigned int milliseconds) {
    struct timespec ts;
    ts.tv_sec  = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
}
