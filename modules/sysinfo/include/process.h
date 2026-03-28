//
// Created by martin on 28.03.2026.
//

#ifndef CLUELESS_TASK_MGR_PROCESS_H
#define CLUELESS_TASK_MGR_PROCESS_H
#include <stddef.h>

#include "common.h"

typedef struct {
    int           pid;
    char          name[256];
    unsigned long utime;  // user time ticks
    unsigned long stime;  // kernel time ticks
    unsigned long rss_kb; // resident set size, the portion of memory (measured in kilobytes) occupied by a process that is held in RAM.
} process_t;

typedef struct {
    process_t *processes;
    size_t     count;
    size_t     capacity;
} process_array_t;

stat_result_t process_fetch_all(process_array_t *array);

void process_array_free(process_array_t *array);

#endif //CLUELESS_TASK_MGR_PROCESS_H
