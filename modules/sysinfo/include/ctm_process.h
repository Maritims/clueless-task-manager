//
// Created by martin on 28.03.2026.
//

#ifndef CLUELESS_TASK_MGR_PROCESS_H
#define CLUELESS_TASK_MGR_PROCESS_H
#include <stddef.h>

typedef struct {
    unsigned int  pid;
    unsigned int  uid;
    char          username[256];
    char          state;
    char          name[256];
    unsigned long utime;  // user time ticks
    unsigned long stime;  // kernel time ticks
    unsigned long rss_kb; // resident set size, the portion of memory (measured in kilobytes) occupied by a process that is held in RAM.
} CtmProcess;

typedef struct {
    CtmProcess *elements;
    size_t      count;
    size_t      capacity;
} CtmProcessArray;

int ctm_process_array_init(CtmProcessArray *array);

int ctm_process_array_destroy(CtmProcessArray *array);

int process_array_push(CtmProcessArray *array, const CtmProcess *item);

int ctm_process_fetch_all(CtmProcessArray *array, int include_all_users);

int ctm_process_translate_state(const CtmProcess *process, char *out_state);

#endif //CLUELESS_TASK_MGR_PROCESS_H
