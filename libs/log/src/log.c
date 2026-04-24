#include "log/log.h"

/**
 * @internal
 * @brief Generates an ISO 8601 timestamp string in the local timezone.
 * @return A pointer to a static buffer containing the formatted time.
 * @note This uses a static buffer, so it is not thread-safe in ANSI C.
 */
const char* ctm_get_time(void) {
    static char buffer[21];
    time_t      now;
    struct tm*  t;

    time(&now);
    t = localtime(&now);

    /* Format: YYYY-MM-DD HH:MM:SS */
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", t);
    return buffer;
}

const char* ctm_get_level_str(const CtmLogLevel level) {
    switch (level) {
        case CTM_LOG_LEVEL_DEBUG:
            return "DEBUG";
        case CTM_LOG_LEVEL_INFO:
            return "INFO";
        case CTM_LOG_LEVEL_WARN:
            return "WARNING";
        case CTM_LOG_LEVEL_ERROR:
            return "ERROR";
        default:
            return NULL;
    }
}

void ctm_log(const CtmLogLevel level, const char* file, const int line, const char* func, const char* fmt, ...) {
    va_list args;

    fprintf(stderr, "%s [%s] %s:%d - %s: ", ctm_get_time(), ctm_get_level_str(level), file, line, func);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
    fflush(stderr);
}