#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ctm/ctm.h"

struct CPU {
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;
};

CPU* cpu_alloc(void) {
    CPU* cpu;

    cpu = malloc(sizeof(CPU));
    if (cpu == NULL) {
        fprintf(stderr, "cpu_alloc: Failed to allocate memory for CPU: %s\n", strerror(errno));
        return NULL;
    }
    return cpu;
}

int cpu_capture(CPU* cpu) {
    FILE*  fp;
    char   buffer[1024];
    size_t bytes_read;
    char*  line_buffer;

    if (cpu == NULL) {
        fprintf(stderr, "cpu_populate: cpu cannot be NULL\n");
        return -1;
    }

    fp = fopen("/proc/stat", "r");
    if (!fp) {
        fprintf(stderr, "ctm_proc_stat_update: Failed to open /proc/stat: %s\n", strerror(errno));
        return -1;
    }

    bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    if (bytes_read <= 0) {
        fprintf(stderr, "ctm_proc_stat_update: Failed to read /proc/stat: %s\n", strerror(errno));
        return -1;
    }

    line_buffer = buffer;
    if (strncmp(buffer, "cpu", 3) != 0) {
        fprintf(stderr, "Invalid /proc/stat format: %s\n", buffer);
        return -1;
    }
    line_buffer += 5; /* Run past the text "cpu  " (two following whitespace characters) before we start parsing the numbers. */

    cpu->user    = strtoul(line_buffer, &line_buffer, 10);
    cpu->nice    = strtoul(line_buffer, &line_buffer, 10);
    cpu->system  = strtoul(line_buffer, &line_buffer, 10);
    cpu->idle    = strtoul(line_buffer, &line_buffer, 10);
    cpu->iowait  = strtoul(line_buffer, &line_buffer, 10);
    cpu->irq     = strtoul(line_buffer, &line_buffer, 10);
    cpu->softirq = strtoul(line_buffer, &line_buffer, 10);

    return 0;
}

CPU* cpu_get(void) {
    CPU* cpu = cpu_alloc();
    if (cpu == NULL) {
        fprintf(stderr, "cpu_get: Failed to allocate memory for CPU: %s\n", strerror(errno));
        return NULL;
    }
    if (cpu_capture(cpu) != 0) {
        fprintf(stderr, "cpu_get: Failed to populate CPU: %s\n", strerror(errno));
        cpu_free(cpu);
        return NULL;
    }
    return cpu;
}

void cpu_free(CPU* cpu) {
    if (cpu) {
        free(cpu);
    }
}

size_t cpu_size(void) {
    return sizeof(CPU);
}

unsigned long cpu_get_idle_time(const CPU* cpu) {
    if (cpu == NULL) {
        fprintf(stderr, "ctm_cpu_metrics_get_idle_time: metrics cannot be NULL\n");
        return -1;
    }
    return cpu->idle;
}

unsigned long cpu_get_total_time(const CPU* cpu) {
    if (cpu == NULL) {
        fprintf(stderr, "ctm_cpu_metrics_get_total_time: metrics cannot be NULL\n");
        return -1;
    }
    return cpu->user + cpu->nice + cpu->system + cpu->idle + cpu->iowait + cpu->irq + cpu->softirq;
}

unsigned long cpu_get_user_usage(const CPU* current, const CPU* previous) {
    (void) current;
    (void) previous;
    return 0L;
}

unsigned long cpu_get_system_usage(const CPU* current, const CPU* previous) {
    (void) current;
    (void) previous;
    return 0L;
}

unsigned long cpu_get_total_usage(const CPU* current, const CPU* previous) {
    unsigned long current_total_time, current_idle_time, previous_total_time, previous_idle_time, total_time_delta, idle_time_delta;

    if (current == NULL) {
        fprintf(stderr, "ctm_cpu_metrics_get_total_usage: current_metrics cannot be NULL\n");
        return -1;
    }
    if (previous == NULL) {
        fprintf(stderr, "ctm_cpu_metrics_get_total_usage: previous_metrics cannot be NULL\n");
        return -1;
    }

    current_total_time  = cpu_get_total_time(current);
    current_idle_time   = cpu_get_idle_time(current);
    previous_total_time = cpu_get_total_time(previous);
    previous_idle_time  = cpu_get_idle_time(previous);
    total_time_delta    = current_total_time - previous_total_time;

    if (total_time_delta == 0) {
        fprintf(stderr, "ctm_cpu_metrics_get_total_usage: total_time_delta is 0\n");
        return -1;
    }

    idle_time_delta = current_idle_time - previous_idle_time;

    return ((total_time_delta - idle_time_delta) * 10000UL) / total_time_delta;
}
