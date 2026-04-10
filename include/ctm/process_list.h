#ifndef CTM_PROCESS_LIST_H
#define CTM_PROCESS_LIST_H

#ifndef INSIDE_CTM_H
#error "Only ctm/ctm.h can be included directly!"
#endif

#include <stddef.h>

#include "process.h"

typedef struct ProcessList ProcessList;

ProcessList* process_list_alloc(void);
void         process_list_free(ProcessList* list);
size_t       process_list_refresh(ProcessList* list);
size_t       process_list_get_count(const ProcessList* list);
Process*     process_list_find_by_pid(const ProcessList* list, unsigned int pid);

#endif
