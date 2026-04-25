#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "metrics/ctm_cpu_metrics.h"
#include "internal/ctm_cpu_metrics_internal.h"

ctm_cpu_metrics_status_t ctm_cpu_metrics_read(ctm_cpu_metrics_t* out_value) {
    FILE*  fp;
    char   buffer[1024];
    size_t bytes_read;
    char*  line_buffer;

    if (out_value == NULL) {
        return CTM_CPU_METRICS_ERR_INVALID_ARG;
    }

    fp = fopen("/proc/stat", "r");
    if (!fp) {
        return CTM_CPU_METRICS_ERR_OPEN;
    }


    bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    if (bytes_read <= 0) {
        return CTM_CPU_METRICS_ERR_READ;
    }

    line_buffer = buffer;
    if (strncmp(buffer, "cpu", 3) != 0) {
        return CTM_CPU_METRICS_ERR_PARSE;
    }
    line_buffer += 5; /* Run past the text "cpu  " (two following whitespace characters) before we start parsing the numbers. */

    out_value->user       = strtoul(line_buffer, &line_buffer, 10);
    out_value->nice       = strtoul(line_buffer, &line_buffer, 10);
    out_value->system     = strtoul(line_buffer, &line_buffer, 10);
    out_value->idle       = strtoul(line_buffer, &line_buffer, 10);
    out_value->iowait     = strtoul(line_buffer, &line_buffer, 10);
    out_value->irq        = strtoul(line_buffer, &line_buffer, 10);
    out_value->softirq    = strtoul(line_buffer, &line_buffer, 10);
    out_value->steal      = strtoul(line_buffer, &line_buffer, 10);
    out_value->guest      = strtoul(line_buffer, &line_buffer, 10);
    out_value->guest_nice = strtoul(line_buffer, &line_buffer, 10);

    return CTM_CPU_METRICS_SUCCESS;
}

ctm_cpu_metrics_status_t ctm_cpu_metrics_destroy(ctm_cpu_metrics_t* cpu_metrics) {
    if (!cpu_metrics) {
        return CTM_CPU_METRICS_ERR_INVALID_ARG;
    }

    free(cpu_metrics);
    return CTM_CPU_METRICS_SUCCESS;
}

ctm_cpu_metrics_status_t ctm_cpu_metrics_get_sizeof(size_t* out_value) {
    if (out_value == NULL) {
        return CTM_CPU_METRICS_ERR_INVALID_ARG;
    }
    *out_value = sizeof(ctm_cpu_metrics_t);
    return CTM_CPU_METRICS_SUCCESS;
}

ctm_cpu_metrics_status_t ctm_cpu_metrics_get_idle_time(const ctm_cpu_metrics_t* cpu_metrics, unsigned long* out_value) {
    if (cpu_metrics == NULL) {
        return CTM_CPU_METRICS_ERR_INVALID_ARG;
    }
    *out_value = cpu_metrics->idle;
    return CTM_CPU_METRICS_SUCCESS;
}

ctm_cpu_metrics_status_t ctm_cpu_metrics_get_total_time(const ctm_cpu_metrics_t* cpu_metrics, unsigned long* out_value) {
    if (cpu_metrics == NULL || out_value == NULL) {
        return CTM_CPU_METRICS_ERR_INVALID_ARG;
    }
    *out_value = cpu_metrics->user + cpu_metrics->nice + cpu_metrics->system + cpu_metrics->idle + cpu_metrics->iowait + cpu_metrics->irq + cpu_metrics->softirq;
    return CTM_CPU_METRICS_SUCCESS;
}

ctm_cpu_metrics_status_t ctm_cpu_metrics_get_user_usage(const ctm_cpu_metrics_t* current_metrics, const ctm_cpu_metrics_t* previous_metrics, const unsigned long* out_value) {
    (void) current_metrics;
    (void) previous_metrics;
    (void) out_value;
    return 0L;
}

ctm_cpu_metrics_status_t ctm_cpu_metrics_get_system_usage(const ctm_cpu_metrics_t* current_metrics, const ctm_cpu_metrics_t* previous_metrics, const unsigned long* out_value) {
    (void) current_metrics;
    (void) previous_metrics;
    (void) out_value;
    return 0L;
}

ctm_cpu_metrics_status_t ctm_cpu_metrics_get_total_usage(const ctm_cpu_metrics_t* current_metrics, const ctm_cpu_metrics_t* previous_metrics, unsigned long* out_value) {
    unsigned long previous_total_time;
    unsigned long current_total_time;
    unsigned long previous_idle_time;
    unsigned long current_idle_time;
    unsigned long idle_delta;
    unsigned long total_delta;
    unsigned long average_idle_time;

    if (current_metrics == NULL || previous_metrics == NULL || out_value == NULL) {
        return CTM_CPU_METRICS_ERR_INVALID_ARG;
    }

    previous_idle_time = previous_metrics->idle;
    current_idle_time  = current_metrics->idle;
    ctm_cpu_metrics_get_total_time(previous_metrics, &previous_total_time);
    ctm_cpu_metrics_get_total_time(current_metrics, &current_total_time);

    if (previous_total_time == 0 && current_total_time == 0) {
        return CTM_CPU_METRICS_ERR_INTERNAL;
    }

    idle_delta  = current_idle_time - previous_idle_time;
    total_delta = current_total_time - previous_total_time;

    /* Do not divide by zero! */
    if (total_delta == 0) {
        return 0;
    }

    average_idle_time = (idle_delta * 100000) / total_delta;
    *out_value       = 100000 - average_idle_time;
    return CTM_CPU_METRICS_SUCCESS;
}
