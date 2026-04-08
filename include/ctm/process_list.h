#ifndef CTM_PROCESS_LIST_H
#define CTM_PROCESS_LIST_H

#ifndef INSIDE_CTM_H
#error "Only ctm/ctm.h can be included directly!"
#endif

#include <stddef.h>

#include "process.h"

typedef struct ProcessList ProcessList;

ProcessList* process_list_get(void);
void         process_list_free(ProcessList* process_list);
size_t       process_list_get_count(const ProcessList* process_list);
Process*     process_list_get_process(const ProcessList* process_list, unsigned int pid);

#endif
