//
// Created by martin on 26.03.2026.
//

#include "ctm_cpu.h"
#include "cio.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct CtmCpuStats {
    /**
     * @brief time spent in user mode
     */
    unsigned long user;
    /**
     * @brief time spent processing nice processes in user mode
     */
    unsigned long nice;
    /**
     * @brief time spent executing kernel code
     */
    unsigned long system;
    /**
     * @brief time spent idle
     */
    unsigned long idle;
    /**
     * @brief time spent waiting for I/O
     */
    unsigned long iowait;
    /**
     * @brief time spent servicing interrupts
     */
    unsigned long irq;
    /**
     * @brief time spent servicing software interrupts
     */
    unsigned long softirq;

    /**
     * @brief The average idle percentage scaled by 100 (e.g. 1234 = 12.34%).
     */
    unsigned int average_idle_scaled;
    /**
     * @brief The CPU usage percentage scaled by 100 (e.g. 8766 = 87.66%). (
     */
    unsigned int average_usage_scaled;
};

/**
 * @brief Calculates the average idle percentage using fixed-point math with 2 decimal places.
 * @param user See @ref CtmProcStat::user
 * @param nice See @ref CtmProcStat::nice
 * @param system See @ref CtmProcStat::system
 * @param idle See @ref CtmProcStat::idle
 * @param iowait See @ref CtmProcStat::iowait
 * @param irq See @ref CtmProcStat::irq
 * @param softirq See @ref CtmProcStat::softirq
 */
static unsigned int calculate_average_idle_scaled(
    const unsigned long long user,
    const unsigned long long nice,
    const unsigned long long system,
    const unsigned long long idle,
    const unsigned long long iowait,
    const unsigned long long irq,
    const unsigned long long softirq
) {
    const unsigned long long total = user + nice + system + idle + iowait + irq + softirq;
    if (total == 0) {
        return 0;
    }

    // Scale by 10.000 because:
    // 100 for the percentage conversion.
    // 100 for the 2 decimal places of precision.
    return idle * 10000ULL / total;
}

/**
 * @brief Calculates the CPU usage using fixed-point math with 2 decimal places (e.g. 10,000 = 100.00%).
 * @param average_idle_scaled The average idle percentage scaled by 100 (e.g. 9000 = 90.00%).
 * @return The usage percentage scaled by 100 (e.g. 7356 = 73.56%).
 */
static unsigned int calculate_usage_scaled(const unsigned int average_idle_scaled) {
    return 10000 - average_idle_scaled;
}

CtmCpuStats* ctm_cpu_stats_new(void) {
    CtmCpuStats* stats = malloc(sizeof(CtmCpuStats));
    if (!stats) {
        fprintf(stderr, "Failed to allocate memory for CPU stats: %d\n", errno);
        return NULL;
    }

    *stats = (CtmCpuStats){0};
    return stats;
}

void ctm_cpu_stats_free(CtmCpuStats* cpu_stats) {
    if (cpu_stats) {
        free(cpu_stats);
    }
}

CtmCpuStats* ctm_cpu_stats_from_kernel(void) {
    size_t file_size = 0;
    char* proc_stat_buffer = cio_read_file("/proc/stat", &file_size);
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

    CtmCpuStats* cpu_stats = ctm_cpu_stats_new();
    if (!cpu_stats) {
        fprintf(stderr, "%s: Failed to allocate memory for CPU stats\n", __func__);
        free(proc_stat_buffer);
        return NULL;
    }

    cpu_stats->user                 = strtoul(buffer_ptr, &buffer_ptr, 10);
    cpu_stats->nice                 = strtoul(buffer_ptr, &buffer_ptr, 10);
    cpu_stats->system               = strtoul(buffer_ptr, &buffer_ptr, 10);
    cpu_stats->idle                 = strtoul(buffer_ptr, &buffer_ptr, 10);
    cpu_stats->iowait               = strtoul(buffer_ptr, &buffer_ptr, 10);
    cpu_stats->irq                  = strtoul(buffer_ptr, &buffer_ptr, 10);
    cpu_stats->softirq              = strtoul(buffer_ptr, &buffer_ptr, 10);
    cpu_stats->average_idle_scaled  = calculate_average_idle_scaled(cpu_stats->user, cpu_stats->nice, cpu_stats->system, cpu_stats->idle, cpu_stats->iowait, cpu_stats->irq, cpu_stats->softirq);
    cpu_stats->average_usage_scaled = calculate_usage_scaled(cpu_stats->average_idle_scaled);

    free(proc_stat_buffer);
    return cpu_stats;
}

unsigned long ctm_cpu_stats_get_user(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->user;
}

unsigned long ctm_cpu_stats_get_nice(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->nice;
}

unsigned long ctm_cpu_stats_get_system(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->system;
}

unsigned long ctm_cpu_stats_get_idle(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->idle;
}

unsigned long ctm_cpu_stats_get_iowait(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->iowait;
}

unsigned long ctm_cpu_stats_get_irq(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->irq;
}

unsigned long ctm_cpu_stats_get_softirq(const CtmCpuStats* cpu_stats) {
    if (!cpu_stats) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return -1;
    }
    return cpu_stats->softirq;
}

unsigned int ctm_cpu_stats_get_usage_scaled(const CtmCpuStats* cpu_stats) {
    if (cpu_stats == NULL) {
        fprintf(stderr, "%s: cpu_stats cannot be NULL\n", __func__);
        return 0;
    }
    return cpu_stats->average_usage_scaled;
}
