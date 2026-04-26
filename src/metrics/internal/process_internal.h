#ifndef CTM_CTM_PROCESS_METRICS_INTERNAL_H
#define CTM_CTM_PROCESS_METRICS_INTERNAL_H
#include "list.h"

struct process {
    unsigned int    pid;
    char            name[256];
    char            state;
    unsigned long   utime;
    unsigned long   stime;
    unsigned long   start_time;
    unsigned long   rss;
    unsigned long   uid;
    char            user[256];
    int             is_active;

    list_node_t node;
};

#endif
