#ifndef CTM_PROCESS_TAB_H
#define CTM_PROCESS_TAB_H
#include <stddef.h>

#include "metrics/ctm_process_metrics.h"

typedef struct ProcessTab ProcessTab;

int process_tab_create(ProcessTab* out_value);
int process_tab_destroy(ProcessTab* tab);
int process_tab_update(ProcessTab* tab, ctm_process_metrics_t* process_metrics_list, size_t* out_value);
int process_tab_register(ProcessTab* tab, void* tab_container);

#endif
