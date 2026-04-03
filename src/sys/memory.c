//
// Created by martin on 26.03.2026.
//

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ctm/sys.h"

struct CtmMemory {
    unsigned long available;
    unsigned long free;
    unsigned long total;
    double        used_percent;
};

CtmMemory* CtmMemory_Create(void) {
    CtmMemory* handle = malloc(sizeof(CtmMemory));
    if (!handle) {
        fprintf(stderr, "%s: Failed to allocate memory for memory stats: %d\n", __func__, errno);
        return NULL;
    }

    handle->available    = 0;
    handle->free         = 0;
    handle->total        = 0;
    handle->used_percent = 0.0;

    return handle;
}

void CtmMemory_Destroy(CtmMemory* handle) {
    if (handle) {
        handle->available    = 0;
        handle->free         = 0;
        handle->total        = 0;
        handle->used_percent = 0.0;
        free(handle);
    }
}

CtmMemory* CtmMemory_LoadFromKernel(void) {
    FILE* fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        fprintf(stderr, "Failed to open /proc/meminfo: %s\n", strerror(errno));
        return NULL;
    }

    char       line[256];
    int        read_available = 0;
    int        read_free      = 0;
    int        read_total     = 0;
    CtmMemory* handle         = malloc(sizeof(CtmMemory));
    if (!handle) {
        fprintf(stderr, "Failed to allocate memory for memory stats: %d\n", errno);
        fclose(fp);
        return NULL;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemAvailable:", 13) == 0) {
            char* end         = NULL;
            errno             = 0;
            handle->available = strtoul(line + 13, &end, 10);
            if (errno == 0 && end != line + 13) read_available = 1;
        }
        if (strncmp(line, "MemFree:", 8) == 0) {
            char* end    = NULL;
            errno        = 0;
            handle->free = strtoul(line + 8, &end, 10);
            if (errno == 0 && end != line + 8) read_free = 1;
        }
        if (strncmp(line, "MemTotal:", 9) == 0) {
            char* end     = NULL;
            errno         = 0;
            handle->total = strtoul(line + 9, &end, 10);
            if (errno == 0 && end != line + 9) read_total = 1;
        }
    }

    fclose(fp);

    if (!read_available || !read_free || !read_total) {
        fprintf(stderr, "Failed to read /proc/meminfo: %s\n", strerror(errno));
        free(handle);
        return NULL;
    }

    handle->used_percent = handle->total == 0 ? 0.0 : ((double) handle->total - (double) handle->available) / (double) handle->total * 100.0;

    return handle;
}

unsigned long CtmMemory_GetAvailable(CtmMemory* handle) {
    if (!handle) {
        fprintf(stderr, "ctm_memory_stats_get_available: Invalid argument: NULL\n");
        return -1;
    }
    return handle->available;
}

unsigned long CtmMemory_GetFree(CtmMemory* handle) {
    if (!handle) {
        fprintf(stderr, "ctm_memory_stats_get_free: Invalid argument: NULL\n");
        return -1;
    }
    return handle->free;
}

unsigned long CtmMemory_GetTotal(CtmMemory* handle) {
    if (!handle) {
        fprintf(stderr, "%s: Invalid argument: NULL\n", __func__);
        return -1;
    }
    return handle->total;
}

double CtmMemory_GetUsedPercent(CtmMemory* handle) {
    if (!handle) {
        fprintf(stderr, "%s: Invalid argument: NULL\n", __func__);
        return -1;
    }
    return handle->used_percent;
}
