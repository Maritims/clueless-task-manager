//
// Created by martin on 26.03.2026.
//

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ctm/io.h"
#include "ctm/sys.h"

struct CtmCpu {
    unsigned long user;    // time spent in user mode
    unsigned long nice;    // time spent processing nice processes in use rmode
    unsigned long system;  // time spent executing kernel code
    unsigned long idle;    // time spent idle
    unsigned long iowait;  // time spent waiting for I/O
    unsigned long irq;     // time spent servicing interrupts
    unsigned long softirq; // time spent servicing software interrupts

    unsigned long long total_time;           // time spent on everything, calculated during construction
    unsigned int       average_idle_scaled;  // the average idle percentage scaled by 100 (e.g. 1234 = 12.34%).
    unsigned int       average_usage_scaled; // the cpu usage percentage scaled by 100 (e.g. 8766 = 87.66%), lazy loaded
};

CtmCpu* CtmCpu_Create(void) {
    CtmCpu* handle = malloc(sizeof(CtmCpu));
    if (!handle) {
        fprintf(stderr, "Failed to allocate memory for CPU stats: %d\n", errno);
        return NULL;
    }

    *handle = (CtmCpu){0};
    return handle;
}

void CtmCpu_Destroy(CtmCpu* handle) {
    if (handle) {
        free(handle);
    }
}

CtmCpu* CtmCpu_LoadFromKernel(void) {
    size_t file_size        = 0;
    char*  proc_stat_buffer = cio_read_file("/proc/stat", &file_size);
    if (proc_stat_buffer == NULL) {
        fprintf(stderr, "%s: Failed to read /proc/stat: %s\n", __func__, strerror(errno));
        return NULL;
    }

    if (strlen(proc_stat_buffer) <= 0) {
        fprintf(stderr, "%s: proc_stat_buffer cannot be empty\n", __func__);
        free(proc_stat_buffer);
        return NULL;
    }

    char* buffer_ptr = proc_stat_buffer;
    if (strncmp(buffer_ptr, "cpu", 3) != 0) {
        fprintf(stderr, "Invalid /proc/stat format: %s\n", proc_stat_buffer);
        free(proc_stat_buffer);
        return NULL;
    }
    buffer_ptr += 5; // Run past the text "cpu  " (two following whitespace characters) before we start parsing the numbers.

    CtmCpu* cpu_handle = CtmCpu_Create();
    if (!cpu_handle) {
        fprintf(stderr, "%s: Failed to allocate memory for CPU stats\n", __func__);
        free(proc_stat_buffer);
        return NULL;
    }

    cpu_handle->user    = strtoul(buffer_ptr, &buffer_ptr, 10);
    cpu_handle->nice    = strtoul(buffer_ptr, &buffer_ptr, 10);
    cpu_handle->system  = strtoul(buffer_ptr, &buffer_ptr, 10);
    cpu_handle->idle    = strtoul(buffer_ptr, &buffer_ptr, 10);
    cpu_handle->iowait  = strtoul(buffer_ptr, &buffer_ptr, 10);
    cpu_handle->irq     = strtoul(buffer_ptr, &buffer_ptr, 10);
    cpu_handle->softirq = strtoul(buffer_ptr, &buffer_ptr, 10);

    free(proc_stat_buffer);
    return cpu_handle;
}

unsigned int CtmCpu_GetUsageScaled(CtmCpu* handle) {
    if (handle == NULL) {
        fprintf(stderr, "%s: cpu_stats cannot be NULL\n", __func__);
        return 0;
    }
    if (handle->average_usage_scaled == 0) {
        if (handle->total_time == 0) {
            handle->total_time = handle->user + handle->nice + handle->system + handle->idle + handle->iowait + handle->irq + handle->softirq;
        }
        if (handle->average_idle_scaled == 0) {
            handle->average_idle_scaled = handle->total_time == 0 || handle->idle == 0 ? 0 : handle->idle * 10000ULL / handle->total_time;
        }
        handle->average_usage_scaled = 10000 - handle->average_idle_scaled;
    }
    return handle->average_usage_scaled;
}
