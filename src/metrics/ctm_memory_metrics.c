#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "metrics/ctm_memory_metrics.h"

#include "core/ctm_platform.h"
#include "internal/ctm_memory_metrics_internal.h"

ctm_memory_metrics_status_t ctm_memory_metrics_read(ctm_memory_metrics_t* out_value) {
    FILE*  fp;
    size_t bytes_read;
    char   buffer[1024];
    char* line_buffer;
    char* save_ptr;
    int   read_available = -1, read_free = -1, read_total = -1;

    if (out_value == NULL) {
        return CTM_MEMORY_METRICS_ERR_INVALID_ARG;
    }

    ;
    if ((fp = fopen("/proc/meminfo", "r")) == NULL) {
        return CTM_MEMORY_METRICS_ERR_OPEN;
    }

    bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    if (bytes_read <= 0) {
        return CTM_MEMORY_METRICS_ERR_READ;
    }

    line_buffer = ctm_strtok_r(buffer, "\n", &save_ptr);
    while (line_buffer != NULL) {
        if (strncmp(line_buffer, "MemAvailable:", 13) == 0) {
            out_value->mem_available = strtoul(line_buffer + 13, NULL, 10);
            read_available        = 1;
        } else if (strncmp(line_buffer, "MemFree:", 8) == 0) {
            out_value->mem_free = strtoul(line_buffer + 8, NULL, 10);
            read_free        = 1;
        } else if (strncmp(line_buffer, "MemTotal:", 9) == 0) {
            out_value->mem_total = strtoul(line_buffer + 9, NULL, 10);
            read_total        = 1;
        }
        line_buffer = ctm_strtok_r(NULL, "\n", &save_ptr);
    }

    if (read_available == -1 || read_free == -1 || read_total == -1) {
        return CTM_MEMORY_METRICS_ERR_PARSE;
    }

    out_value->mem_available = 0;
    out_value->mem_free      = 0;
    out_value->mem_total     = 0;

    return CTM_MEMORY_METRICS_SUCCESS;
}

ctm_memory_metrics_status_t ctm_memory_metrics_destroy(ctm_memory_metrics_t* memory_metrics) {
    if (!memory_metrics) {
        return CTM_MEMORY_METRICS_ERR_INVALID_ARG;
    }

    free(memory_metrics);
    return CTM_MEMORY_METRICS_SUCCESS;
}

ctm_memory_metrics_status_t ctm_memory_metrics_get_total(const ctm_memory_metrics_t* memory_metrics, unsigned long* out_value) {
    if (!memory_metrics || !out_value) {
        return CTM_MEMORY_METRICS_ERR_INVALID_ARG;
    }

    *out_value = memory_metrics->mem_total;
    return CTM_MEMORY_METRICS_SUCCESS;
}

ctm_memory_metrics_status_t ctm_memory_metrics_get_free(const ctm_memory_metrics_t* memory_metrics, unsigned long* out_value) {
    if (!memory_metrics || !out_value) {
        return CTM_MEMORY_METRICS_ERR_INVALID_ARG;
    }

    *out_value = memory_metrics->mem_free;
    return CTM_MEMORY_METRICS_SUCCESS;
}

ctm_memory_metrics_status_t ctm_memory_metrics_get_available(const ctm_memory_metrics_t* memory_metrics, unsigned long* out_value) {
    if (!memory_metrics || !out_value) {
        return CTM_MEMORY_METRICS_ERR_INVALID_ARG;
    }

    *out_value = memory_metrics->mem_available;
    return CTM_MEMORY_METRICS_SUCCESS;
}

const char* ctm_memory_metrics_strerror(const ctm_memory_metrics_status_t memory_metrics_status) {
    switch (memory_metrics_status) {
        case CTM_MEMORY_METRICS_SUCCESS:
            return "Success";
        case CTM_MEMORY_METRICS_ERR_INVALID_ARG:
            return "Invalid argument";
        case CTM_MEMORY_METRICS_ERR_OPEN:
            return "Failed to open file";
        case CTM_MEMORY_METRICS_ERR_READ:
            return "Failed to read file";
        case CTM_MEMORY_METRICS_ERR_PARSE:
            return "Failed to parse file";
        case CTM_MEMORY_METRICS_ERR_INTERNAL:
            return "Internal error";
        default:
            return "Unknown error";
    }
}
