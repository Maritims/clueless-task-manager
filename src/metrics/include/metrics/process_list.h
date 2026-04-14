#ifndef CTM_PROCESS_LIST_H
#define CTM_PROCESS_LIST_H

#include <stddef.h>

#include "process.h"

typedef struct ProcessList ProcessList;
typedef int (*             ProcessListForeachFunc)(Process* process);

typedef struct ProcessListObserver {
    void (*on_process_added)(Process* process, void* user_data);
    void (*on_process_updated)(Process* process, void* user_data);
    void (*on_process_removed)(Process* process, void* user_data);
} ProcessListObserver;

ProcessList* process_list_alloc(void);
void         process_list_free(ProcessList* list);
int          process_list_refresh(ProcessList* list, ProcessListObserver* observer, void* user_data, size_t* out_size);
size_t       process_list_get_count(const ProcessList* list);
Process*     process_list_find_by_pid(const ProcessList* list, unsigned int pid);
int          process_list_foreach(const ProcessList* list, ProcessListForeachFunc foreach_func);

#endif
