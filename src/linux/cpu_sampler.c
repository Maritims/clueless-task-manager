#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ctm/ctm.h"

#define CPU_SAMPLER_HISTORY_CAPACITY 100

struct CPUSampler {
    CPU*         history;
    size_t       item_size;
    unsigned int interval_ms;
    int          is_running;

    size_t          head;
    size_t          count;
    pthread_mutex_t lock;
    pthread_t       thread;
};

static void* get_sample_address(const CPUSampler* sampler, const size_t index) {
    /* Cast to unsigned char to allow byte-level arithmetic */
    return (unsigned char *) sampler->history + (index * sampler->item_size);
}

static void* cpu_sampler_thread_func(void* arg) {
    CPUSampler*     sampler = (CPUSampler *) arg;
    struct timespec requested, remaining;

    requested.tv_sec  = sampler->interval_ms / 1000;
    requested.tv_nsec = (sampler->interval_ms % 1000) * 1000000;

    while (sampler->is_running) {
        CPU* current_slot;

        pthread_mutex_lock(&sampler->lock);

        current_slot = (CPU*)get_sample_address(sampler, sampler->head);

        if (cpu_capture(current_slot) == 0) {
            sampler->head = (sampler->head + 1) % CPU_SAMPLER_HISTORY_CAPACITY;
            if (sampler->count < CPU_SAMPLER_HISTORY_CAPACITY) {
                sampler->count++;
            }
        }

        pthread_mutex_unlock(&sampler->lock);

        nanosleep(&requested, &remaining);
    }

    return NULL;
}

CPUSampler* cpu_sampler_create(const unsigned int interval_ms) {
    CPUSampler* sampler;

    sampler = malloc(sizeof(CPUSampler));
    if (sampler == NULL) {
        fprintf(stderr, "cpu_sampler_create: Failed to allocate memory for CPUSampler: %s\n", strerror(errno));
        return NULL;
    }
    if ((sampler->history = malloc(CPU_SAMPLER_HISTORY_CAPACITY * cpu_size())) == NULL) {
        fprintf(stderr, "cpu_sampler_create: Failed to allocate memory for CPUSampler history: %s\n", strerror(errno));
        free(sampler);
        return NULL;
    }

    if (pthread_mutex_init(&sampler->lock, NULL) != 0) {
        fprintf(stderr, "cpu_sampler_create: Failed to initialize mutex: %s\n", strerror(errno));
        cpu_sampler_destroy(sampler);
        return NULL;
    }

    sampler->item_size   = cpu_size();
    sampler->interval_ms = interval_ms;
    sampler->is_running  = 0;
    sampler->head        = 0;

    return sampler;
}

int cpu_sampler_start(CPUSampler* sampler) {
    if (sampler == NULL) {
        fprintf(stderr, "cpu_sampler_start: Invalid argument: NULL\n");
        return -1;
    }

    pthread_mutex_lock(&sampler->lock);

    if (sampler->is_running) {
        pthread_mutex_unlock(&sampler->lock);
        return 0;
    }

    sampler->is_running = 1;
    if (pthread_create(&sampler->thread, NULL, cpu_sampler_thread_func, sampler) != 0) {
        fprintf(stderr, "cpu_sampler_start: Failed to create thread: %s\n", strerror(errno));
        sampler->is_running = 0;
        pthread_mutex_unlock(&sampler->lock);
        return -1;
    }

    pthread_mutex_unlock(&sampler->lock);
    return 0;
}

void cpu_sampler_destroy(CPUSampler* sampler) {
    int was_running;

    if (sampler == NULL) {
        fprintf(stderr, "cpu_sampler_destroy: Invalid argument: NULL\n");
        return;
    }

    pthread_mutex_lock(&sampler->lock);
    was_running = sampler->is_running;
    sampler->is_running = 0;
    pthread_mutex_unlock(&sampler->lock);

    if (was_running) {
        pthread_join(sampler->thread, NULL);
    }

    pthread_mutex_destroy(&sampler->lock);
    free(sampler->history);
    free(sampler);
}

long cpu_sampler_get_avg_usage(CPUSampler* sampler, const size_t window) {
    long usage = -1;

    if (sampler == NULL) {
        fprintf(stderr, "cpu_sampler_get_average_usage: Invalid argument: NULL\n");
        return -1;
    }

    pthread_mutex_lock(&sampler->lock);

    if (sampler->count > window) {
        const size_t current_id  = (sampler->head - 1 + 100) % 100; /* Wrap-around */
        const size_t previous_id = (sampler->head - 1 - window + 100) % 100;
        const void*  current     = get_sample_address(sampler, current_id);
        const void*  previous    = get_sample_address(sampler, previous_id);
        usage                    = (long) cpu_get_total_usage(current, previous);
    }

    pthread_mutex_unlock(&sampler->lock);
    return usage;
}
