#ifndef CTM_PROCESS_H
#define CTM_PROCESS_H

#ifndef INSIDE_CTM_H
#error "Only ctm/ctm.h can be included directly!"
#endif

typedef struct Process Process;

Process*     process_get(unsigned int pid);
void         process_free(Process* process);
unsigned int process_get_pid(const Process* process);
const char*  process_get_name(const Process* process);
const char*  process_get_state(const Process* process);
const char*  process_get_user(const Process* process);
unsigned int process_get_total_time(const Process* process);

#endif
