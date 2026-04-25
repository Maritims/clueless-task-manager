#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "metrics/ctm_memory_metrics.h"
#include "ctm_types.h"
#include "internal/ctm_memory_metrics_internal.h"

ctm_memory_metrics_status_t ctm_memory_metrics_read(ctm_memory_metrics_t *out_value) {
    FILE *fp;
    size_t bytes_read;
    char  buffer[1024];
    char* line_start;
    char* line_end;
    int   read_available = -1, read_free = -1, read_total = -1;

    if (out_value == NULL) {
        return CTM_MEMORY_METRICS_ERR_INVALID_ARG;
    }

    ;
    if ((fp = fopen("/proc/meminfo", "r")) == NULL) {
        return CTM_MEMORY_METRICS_ERR_OPEN;
    }

    bytes_read = fread(buffer, 1, sizeof(buffer) - 1, fp);
    fclose(fp);
    if (bytes_read <= 0) {
        return CTM_MEMORY_METRICS_ERR_READ;
    }

    buffer[bytes_read] = '\0';

    line_start = buffer;
    while (*line_start != '\0') {
        line_end = line_start;
        while (*line_end != '\0' && *line_end != '\n') {
            line_end++;
        }

        if (*line_end == '\n') {
            *line_end = '\0';
        }

        if (strncmp(line_start, "MemAvailable:", 13) == 0) {
            out_value->mem_available = strtoul(line_start + 13, NULL, 10);
            read_available        = 1;
        } else if (strncmp(line_start, "MemFree:", 8) == 0) {
            out_value->mem_free = strtoul(line_start + 8, NULL, 10);
            read_free        = 1;
        } else if (strncmp(line_start, "MemTotal:", 9) == 0) {
            out_value->mem_total = strtoul(line_start + 9, NULL, 10);
            read_total        = 1;
        }

        if (*line_end == '\0') {
            break;
        }

        line_start = line_end + 1;
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
