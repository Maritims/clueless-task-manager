//
// Created by martin on 28.03.2026.
//

#ifndef CLUELESS_TASK_MGR_PROCESS_H
#define CLUELESS_TASK_MGR_PROCESS_H
#include <stddef.h>
#include <time.h>

typedef struct CtmProcess      CtmProcess;
typedef struct CtmProcessArray CtmProcessArray;

// Constructor and destructor.
CtmProcessArray* ctm_process_array_new(void);
void             ctm_process_array_free(CtmProcessArray* array);
CtmProcess*      ctm_process_new(pid_t pid, unsigned int uid, const char* name, const char* state, const char* username, unsigned long utime, unsigned long stime, unsigned long rss_kb);
void             ctm_process_free(CtmProcess* process);

// Factory functions.
CtmProcessArray* ctm_processes_from_kernel(int include_all_users);
CtmProcess*      ctm_process_from_kernel(pid_t pid, int include_all_users);

// Accessors.
size_t            ctm_process_array_get_count(const CtmProcessArray* array);
size_t            ctm_process_array_get_capacity(const CtmProcessArray* array);
const CtmProcess* ctm_process_array_get_element(const CtmProcessArray* array, size_t index);
const CtmProcess* ctm_process_array_get_elements(const CtmProcessArray* array);
pid_t             ctm_process_get_pid(const CtmProcess* process);
const char*       ctm_process_get_name(const CtmProcess* process);
const char*       ctm_process_get_state(const CtmProcess* process);
const char*       ctm_process_get_username(const CtmProcess* process);

// Logic.
int ctm_process_array_push(CtmProcessArray* array, const CtmProcess* item);

#endif //CLUELESS_TASK_MGR_PROCESS_H
