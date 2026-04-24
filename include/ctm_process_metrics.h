#ifndef CTM_PROCESS_METRICS_H
#define CTM_PROCESS_METRICS_H
#include <stddef.h>
#include "ctm_types.h"

typedef enum {
    CTM_PROCESS_METRICS_SUCCESS,
    CTM_PROCESS_METRICS_ERR_INVALID_ARG,
    CTM_PROCESS_METRICS_ERR_NOT_FOUND,
    CTM_PROCESS_METRICS_ERR_OPEN,
    CTM_PROCESS_METRICS_ERR_READ,
    CTM_PROCESS_METRICS_ERR_PARSE,
    CTM_PROCESS_METRICS_ERR_INTERNAL
} ctm_process_metrics_status_t;

ctm_process_metrics_status_t ctm_process_metrics_read(unsigned int pid, ctm_process_metrics_t* out_value);
ctm_process_metrics_status_t ctm_process_metrics_destroy(ctm_process_metrics_t* process_metrics);
ctm_process_metrics_status_t ctm_process_metrics_get_sizeof(size_t* out_value);
ctm_process_metrics_status_t ctm_process_metrics_get_pid(const ctm_process_metrics_t* process_metrics, unsigned int* out_value);
ctm_process_metrics_status_t ctm_process_metrics_get_name(const ctm_process_metrics_t* process_metrics, char* out_value, size_t out_value_size);
ctm_process_metrics_status_t ctm_process_metrics_get_state(const ctm_process_metrics_t* process_metrics, char* out_value, size_t out_value_size);
ctm_process_metrics_status_t ctm_process_metrics_get_user(const ctm_process_metrics_t* process_metrics, char* out_value, size_t out_value_size);
ctm_process_metrics_status_t ctm_process_metrics_get_is_active(const ctm_process_metrics_t* process_metrics, int* out_value);
ctm_process_metrics_status_t ctm_process_metrics_set_is_active(ctm_process_metrics_t* self, int is_active);
ctm_process_metrics_status_t ctm_process_metrics_get_total_time(const ctm_process_metrics_t* process_metrics, unsigned int* out_value);

const char* ctm_process_metrics_strerror(ctm_process_metrics_status_t process_metrics_status);

#endif
