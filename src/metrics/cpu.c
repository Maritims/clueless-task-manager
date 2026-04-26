#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "internal/cpu_internal.h"

cpu_result_t cpu_read(cpu_t* out)
{
    FILE*  fp;
    char   buffer[1024];
    size_t bytes_read;
    char*  line_buffer;

    if (out == NULL) {
        return CPU_ERR_INVALID_ARG;
    }

    fp = fopen("/proc/stat", "r");
    if (!fp) {
        return CPU_ERR_OPEN;
    }


    bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    if (bytes_read <= 0) {
        return CPU_ERR_READ;
    }

    line_buffer = buffer;
    if (strncmp(buffer, "cpu", 3) != 0) {
        return CPU_ERR_PARSE;
    }
    line_buffer += 5; /* Run past the text "cpu  " (two following whitespace characters) before we start parsing the numbers. */

    out->user       = strtoul(line_buffer, &line_buffer, 10);
    out->nice       = strtoul(line_buffer, &line_buffer, 10);
    out->system     = strtoul(line_buffer, &line_buffer, 10);
    out->idle       = strtoul(line_buffer, &line_buffer, 10);
    out->iowait     = strtoul(line_buffer, &line_buffer, 10);
    out->irq        = strtoul(line_buffer, &line_buffer, 10);
    out->softirq    = strtoul(line_buffer, &line_buffer, 10);
    out->steal      = strtoul(line_buffer, &line_buffer, 10);
    out->guest      = strtoul(line_buffer, &line_buffer, 10);
    out->guest_nice = strtoul(line_buffer, &line_buffer, 10);

    return CPU_SUCCESS;
}

cpu_result_t cpu_destroy(cpu_t* cpu)
{
    if (cpu) {
        free(cpu);
        return CPU_SUCCESS;
    }
    return CPU_ERR_INVALID_ARG;
}

cpu_result_t cpu_sizeof(size_t* out)
{
    if (out) {
        *out = sizeof(cpu_t);
        return CPU_SUCCESS;
    }
    return CPU_ERR_INVALID_ARG;
}

cpu_result_t cpu_idle_time(const cpu_t*   cpu,
                           unsigned long* out)
{
    if (cpu) {
        *out = cpu->idle;
        return CPU_SUCCESS;
    }
    return CPU_ERR_INVALID_ARG;
}

cpu_result_t cpu_total_time(const cpu_t*   cpu,
                            unsigned long* out)
{
    if (cpu && out) {
        *out = cpu->user + cpu->nice + cpu->system + cpu->idle + cpu->iowait + cpu->irq + cpu->softirq;
        return CPU_SUCCESS;
    }
    return CPU_ERR_INVALID_ARG;
}

cpu_result_t cpu_user_usage(const cpu_t*         curr,
                            const cpu_t*         prev,
                            const unsigned long* out)
{
    (void) curr;
    (void) prev;
    (void) out;
    return 0L;
}

cpu_result_t cpu_system_usage(const cpu_t*         curr,
                              const cpu_t*         prev,
                              const unsigned long* out)
{
    (void) curr;
    (void) prev;
    (void) out;
    return 0L;
}

cpu_result_t cpu_total_usage(const cpu_t*   curr,
                             const cpu_t*   prev,
                             unsigned long* out)
{
    unsigned long previous_total_time;
    unsigned long current_total_time;
    unsigned long previous_idle_time;
    unsigned long current_idle_time;
    unsigned long idle_delta;
    unsigned long total_delta;
    unsigned long average_idle_time;

    if (curr == NULL || prev == NULL || out == NULL) {
        return CPU_ERR_INVALID_ARG;
    }

    previous_idle_time = prev->idle;
    current_idle_time  = curr->idle;
    cpu_total_time(prev, &previous_total_time);
    cpu_total_time(curr, &current_total_time);

    if (previous_total_time == 0 && current_total_time == 0) {
        return CPU_ERR_INTERNAL;
    }

    idle_delta  = current_idle_time - previous_idle_time;
    total_delta = current_total_time - previous_total_time;

    /* Do not divide by zero! */
    if (total_delta == 0) {
        return 0;
    }

    average_idle_time = (idle_delta * 100000) / total_delta;
    *out              = 100000 - average_idle_time;
    return CPU_SUCCESS;
}
