#ifndef CTM_LOGGING_H
#define CTM_LOGGING_H
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

/* Cross-compiler attribute for unused functions */
#if defined(__GNUC__) || defined(__clang__)
#define CTM_ATTRIBUTE_UNUSED __attribute__((unused))
#else
#define CTM_ATTRIBUTE_UNUSED
#endif

typedef enum {
    CTM_LOG_LEVEL_DEBUG,
    CTM_LOG_LEVEL_INFO,
    CTM_LOG_LEVEL_WARN,
    CTM_LOG_LEVEL_ERROR,
    CTM_LOG_LEVEL_NONE
} CtmLogLevel;

#ifndef CTM_LOG_THRESHOLD
#define CTM_LOG_THRESHOLD CTM_LOG_LEVEL_DEBUG
#endif

/**
 * @internal
 * @brief Generates an ISO 8601 timestamp string in the local timezone.
 * @return A pointer to a static buffer containing the formatted time.
 * @note This uses a static buffer, so it is not thread-safe in ANSI C.
 */
static const char* ctm_get_time(void) {
    static char buffer[21];
    time_t      now;
    struct tm*  t;

    time(&now);
    t = localtime(&now);

    /* Format: YYYY-MM-DD HH:MM:SS */
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", t);
    return buffer;
}

static const char* ctm_get_level_str(CtmLogLevel level) {
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

static CTM_ATTRIBUTE_UNUSED void ctm_log(const CtmLogLevel level, const char* file, const int line, const char* func, const char* fmt, ...) {
    va_list args;

    fprintf(stderr, "%s [%s] %s:%d - %s: ", ctm_get_time(), ctm_get_level_str(level), file, line, func);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
    fflush(stderr);
}

#define LOG(level, func, msg) \
    ctm_log(level, __FILE__, __LINE__, func, msg)

/* We add 'stderr ,' inside the expansion so 'fprintf msg' becomes 'fprintf (stderr, ...)' */
#define LOG_DEBUG(func, msg) LOG(CTM_LOG_LEVEL_DEBUG, func, msg)
#define LOG_INFO(func, msg)  LOG(CTM_LOG_LEVEL_INFO,  func, msg)
#define LOG_WARN(func, msg)  LOG(CTM_LOG_LEVEL_WARN,  func, msg)
#define LOG_ERROR(func, msg) LOG(CTM_LOG_LEVEL_ERROR, func, msg)

/**
 * @brief Validates a pointer argument.
 * If NULL, logs an error, sets errno, and returns -1.
 * * @param arg     The pointer to check.
 * @param argName The string name of the argument for the error message.
 */
#define RETURN_INT_IF_ARG_IS_NULL(func, arg, argName) \
    do { \
        if ((arg) == NULL) { \
            ctm_log(CTM_LOG_LEVEL_ERROR, __FILE__, __LINE__, func, "Argument '%s' cannot be NULL", (argName)); \
            errno = EINVAL; \
            return -1; \
        } \
    } while (0)

#define RETURN_VOID_IF_ARG_IS_NULL(func, arg, argName) \
    do { \
        if ((arg) == NULL) { \
            ctm_log(CTM_LOG_LEVEL_ERROR, __FILE__, __LINE__, func, "Argument '%s' cannot be NULL", (argName)); \
            errno = EINVAL; \
            return; \
        } \
    } while (0)


#endif
