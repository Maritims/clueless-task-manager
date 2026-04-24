#ifndef CTM_CPU_METRICS_H
#define CTM_CPU_METRICS_H
#include "ctm_types.h"

#include <stddef.h>

typedef enum {
    CTM_CPU_METRICS_SUCCESS,
    CTM_CPU_METRICS_ERR_INVALID_ARG,
    CTM_CPU_METRICS_ERR_OPEN,
    CTM_CPU_METRICS_ERR_READ,
    CTM_CPU_METRICS_ERR_PARSE,
    CTM_CPU_METRICS_ERR_INTERNAL
} ctm_cpu_metrics_status_t;

ctm_cpu_metrics_status_t ctm_cpu_metrics_read(ctm_cpu_metrics_t* out_value);
ctm_cpu_metrics_status_t ctm_cpu_metrics_destroy(ctm_cpu_metrics_t* cpu_metrics);
ctm_cpu_metrics_status_t ctm_cpu_metrics_get_sizeof(size_t* out_value);
ctm_cpu_metrics_status_t ctm_cpu_metrics_get_total_time(const ctm_cpu_metrics_t* cpu_metrics, unsigned long* out_value);
ctm_cpu_metrics_status_t ctm_cpu_metrics_get_idle_time(const ctm_cpu_metrics_t* cpu_metrics, unsigned long* out_value);
ctm_cpu_metrics_status_t ctm_cpu_metrics_get_user_usage(const ctm_cpu_metrics_t* current_metrics, const ctm_cpu_metrics_t* previous_metrics, const unsigned long* out_value);
ctm_cpu_metrics_status_t ctm_cpu_metrics_get_system_usage(const ctm_cpu_metrics_t* current_metrics, const ctm_cpu_metrics_t* previous_metrics, const unsigned long* out_value);
ctm_cpu_metrics_status_t ctm_cpu_metrics_get_total_usage(const ctm_cpu_metrics_t* current_metrics, const ctm_cpu_metrics_t* previous_metrics, unsigned long* out_value);

#endif
