//
// Created by martin on 26.03.2026.
//

#include <stdio.h>
#include <time.h>

#include <errno.h>
#include <stdlib.h>

#include "ctm/sys.h"

struct CtmSystemInfo {
    CtmCpu*          cpu;
    CtmMemory*       memory;
    CtmProcessArray* processes;
    time_t           timestamp;
};

CtmSystemInfo* CtmSystemInfo_Create(void) {
    CtmSystemInfo* stats = malloc(sizeof(CtmSystemInfo));
    if (!stats) {
        fprintf(stderr, "%s: Failed to allocate memory for systeminfo: %d\n", __func__, errno);
        return NULL;
    }

    stats->cpu       = NULL;
    stats->memory    = NULL;
    stats->processes = NULL;
    stats->timestamp = time(NULL);

    return stats;
}

void CtmSystemInfo_Destroy(CtmSystemInfo* handle) {
    if (handle) {
        CtmCpu_Destroy(handle->cpu);
        CtmMemory_Destroy(handle->memory);
        CtmProcessArray_Destroy(handle->processes);
        handle->timestamp = 0;
        free(handle);
    }
}

CtmSystemInfo* CtmSystemInfo_LoadFromKernel(void) {
    CtmSystemInfo* handle = CtmSystemInfo_Create();

    handle->cpu       = CtmCpu_LoadFromKernel();
    handle->memory    = CtmMemory_LoadFromKernel();
    handle->processes = CtmProcessArray_LoadFromKernel(1);
    handle->timestamp = time(NULL);

    if (!handle->cpu || !handle->memory || !handle->processes) {
        fprintf(stderr, "Failed to fetch system stats\n");
        CtmSystemInfo_Destroy(handle);
        return NULL;
    }

    return handle;
}

CtmCpu* CtmSystemInfo_GetCpu(const CtmSystemInfo* handle) {
    if (!handle) {
        fprintf(stderr, "%s: handle cannot be NULL\n", __func__);
        return NULL;
    }
    return handle->cpu;
}

CtmMemory* CtmSystemInfo_GetMemory(const CtmSystemInfo* handle) {
    if (!handle) {
        fprintf(stderr, "%s: handle cannot be NULL\n", __func__);
        return NULL;
    }
    return handle->memory;
}

CtmProcessArray* CtmSystemInfo_GetProcesses(const CtmSystemInfo* handle) {
    if (!handle) {
        fprintf(stderr, "%s: stats cannot be NULL\n", __func__);
        return NULL;
    }
    return handle->processes;
}
