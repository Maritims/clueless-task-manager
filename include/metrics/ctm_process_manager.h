#ifndef CTM_METRICS_PROCESS_MANAGER_H
#define CTM_METRICS_PROCESS_MANAGER_H

#include "ctm_process_metrics.h"
#include "../core/hash_map.h"

typedef struct ctm_process_manager ctm_process_manager_t;

typedef void (*ProcessManagerCallback)(ctm_process_metrics_t* process_metrics);

typedef enum {
    CTM_PROCESS_MANAGER_SUCCESS,
    CTM_PROCESS_MANAGER_ERR_INVALID_ARG,
    CTM_PROCESS_MANAGER_ERR_OPEN,
    CTM_PROCESS_MANAGER_ERR_NOT_FOUND,
    CTM_PROCESS_MANAGER_ERR_INTERNAL
} ctm_process_manager_status_t;

ctm_process_manager_status_t process_manager_refresh(ctm_process_metrics_t* process_metrics_list,
                                                     size_t                 process_metrics_arr_len,
                                                     ProcessManagerCallback on_process_added,
                                                     ProcessManagerCallback on_process_updated,
                                                     ProcessManagerCallback on_process_removed);

#endif
