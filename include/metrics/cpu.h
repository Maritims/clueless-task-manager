#ifndef METRICS_CPU_H
#define METRICS_CPU_H

#include <stddef.h>

typedef struct cpu cpu_t;

typedef enum {
    CPU_SUCCESS,
    CPU_ERR_INVALID_ARG,
    CPU_ERR_OPEN,
    CPU_ERR_READ,
    CPU_ERR_PARSE,
    CPU_ERR_INTERNAL
} cpu_result_t;

cpu_result_t cpu_read(cpu_t* out);

cpu_result_t cpu_destroy(cpu_t* cpu);

cpu_result_t cpu_sizeof(size_t* out);

cpu_result_t cpu_total_usage(const cpu_t*   curr,
                             const cpu_t*   prev,
                             unsigned long* out);

#endif
