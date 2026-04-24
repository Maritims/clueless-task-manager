#ifndef CTM_MEMORY_METRICS_H
#define CTM_MEMORY_METRICS_H
#include "ctm_types.h"

typedef enum {
    CTM_MEMORY_METRICS_SUCCESS,
    CTM_MEMORY_METRICS_ERR_INVALID_ARG,
    CTM_MEMORY_METRICS_ERR_OPEN,
    CTM_MEMORY_METRICS_ERR_READ,
    CTM_MEMORY_METRICS_ERR_PARSE,
    CTM_MEMORY_METRICS_ERR_INTERNAL
} ctm_memory_metrics_status_t;

ctm_memory_metrics_status_t ctm_memory_metrics_read(ctm_memory_metrics_t* out_value);
ctm_memory_metrics_status_t ctm_memory_metrics_destroy(ctm_memory_metrics_t* memory_metrics);
ctm_memory_metrics_status_t ctm_memory_metrics_get_total(const ctm_memory_metrics_t* memory_metrics, unsigned long* out_value);
ctm_memory_metrics_status_t ctm_memory_metrics_get_free(const ctm_memory_metrics_t* memory_metrics, unsigned long* out_value);
ctm_memory_metrics_status_t ctm_memory_metrics_get_available(const ctm_memory_metrics_t* memory_metrics, unsigned long* out_value);

#endif
