#ifndef CTM_CTM_CPU_METRICS_INTERNAL_H
#define CTM_CTM_CPU_METRICS_INTERNAL_H

struct cpu {
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;
    unsigned long steal;
    unsigned long guest;
    unsigned long guest_nice;
};

#endif
