#ifndef CTM_LOGGING_H
#define CTM_LOGGING_H
#include <errno.h>
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

const char* ctm_get_time(void);
const char* ctm_get_level_str(CtmLogLevel level);
void        ctm_log(CtmLogLevel level,
             const char*        file,
             int                line,
             const char*        func,
             const char*        fmt,
             ...);

#ifndef CTM_LOG_THRESHOLD
#define CTM_LOG_THRESHOLD CTM_LOG_LEVEL_DEBUG
#endif

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

#define RETURN_ZERO_IF_ARG_IS_NULL(func, arg, argName) \
    do { \
        if ((arg) == NULL) { \
            ctm_log(CTM_LOG_LEVEL_ERROR, __FILE__, __LINE__, func, "Argument '%s' cannot be NULL", (argName)); \
            errno = EINVAL; \
            return 0; \
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
