#include "metrics/sampler.h"

#include <errno.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../collections/include/collections/ring_buffer.h"

#define CTM_SAMPLER_CAPACITY 100

struct Sampler {
    RingBuffer*  ring_buffer;
    unsigned int interval_ms;
    int          is_running;

    pthread_mutex_t lock;
    pthread_t       thread;

    SamplerCaptureFunc capture_fn;
    SamplerCallback    on_captured;
};

static void* sampler_thread_fn(void* sampler_ptr) {
    Sampler*        sampler = (Sampler *) sampler_ptr;
    struct timespec requested, remaining;

    requested.tv_sec  = sampler->interval_ms / 1000;
    requested.tv_nsec = (sampler->interval_ms % 1000) * 1000000;

    while (1) {
        void* current_slot;
        int   notify;

        pthread_mutex_lock(&sampler->lock);

        if (!sampler->is_running) {
            pthread_mutex_unlock(&sampler->lock);
            break;
        }

        current_slot = ring_buffer_advance(sampler->ring_buffer);
        notify       = 0;

        if (sampler->capture_fn(current_slot) == 0) {
            notify = 1;
        } else {
            fprintf(stderr, "ctm_sampler_thread_fn: Sampler capture function failed: %s\n", strerror(errno));
        }

        pthread_mutex_unlock(&sampler->lock);
        if (notify && sampler->on_captured != NULL) {
            sampler->on_captured(sampler);
        }

        nanosleep(&requested, &remaining);
    }
    return NULL;
}

Sampler* sampler_create(const unsigned int interval_ms, const size_t item_size, const SamplerCaptureFunc capture_fn) {
    Sampler* sampler;
    int      mutex_init_result;

    if (item_size == 0 || capture_fn == NULL) {
        errno = EINVAL;
        return NULL;
    }

    sampler = malloc(sizeof(Sampler));
    if (sampler == NULL) {
        /* malloc has already set errno */
        return NULL;
    }

    if ((sampler->ring_buffer = ring_buffer_alloc(CTM_SAMPLER_CAPACITY, item_size)) == NULL) {
        /* keep errno from malloc, free sampler and restore errno */
        const int error = errno;
        free(sampler);
        errno = error;
        return NULL;
    }

    if ((mutex_init_result = pthread_mutex_init(&sampler->lock, NULL)) != 0) {
        /* free history and sampler, then assign the result of mutex init to errno */
        ring_buffer_free(sampler->ring_buffer);
        free(sampler);
        errno = mutex_init_result;
        return NULL;
    }

    sampler->interval_ms = interval_ms;
    sampler->capture_fn  = capture_fn;
    sampler->on_captured = NULL;
    sampler->is_running  = 0;

    return sampler;
}

int sampler_destroy(Sampler* sampler) {
    if (sampler == NULL) {
        errno = EINVAL;
        return -1;
    }

    sampler_stop(sampler);
    pthread_mutex_destroy(&sampler->lock);
    ring_buffer_free(sampler->ring_buffer);
    free(sampler);

    return 0;
}

int sampler_start(Sampler* sampler) {
    int thread_create_result;

    if (sampler == NULL) {
        errno = EINVAL;
        return -1;
    }

    pthread_mutex_lock(&sampler->lock);

    if (sampler->is_running) {
        pthread_mutex_unlock(&sampler->lock);
        return 0;
    }

    sampler->is_running = 1;
    if ((thread_create_result = pthread_create(&sampler->thread, NULL, sampler_thread_fn, sampler)) != 0) {
        sampler->is_running = 0;
        pthread_mutex_unlock(&sampler->lock);
        errno = thread_create_result;
        return -1;
    }

    pthread_mutex_unlock(&sampler->lock);
    return 0;
}

int sampler_stop(Sampler* sampler) {
    int was_running, join_result;

    if (sampler == NULL) {
        errno = EINVAL;
        return -1;
    }

    pthread_mutex_lock(&sampler->lock);

    was_running         = sampler->is_running;
    sampler->is_running = 0;

    pthread_mutex_unlock(&sampler->lock);

    if (was_running) {
        if ((join_result = pthread_join(sampler->thread, NULL)) != 0) {
            errno = join_result;
            return -1;
        }
    }

    return 0;
}

int sampler_subscribe(Sampler* sampler, const SamplerCallback subscriber) {
    if (sampler == NULL || subscriber == NULL) {
        errno = EINVAL;
        return -1;
    }

    sampler->on_captured = subscriber;
    return 0;
}

long sampler_get_value(Sampler* sampler, const size_t window, SamplerProcessFunc process_fn) {
    void * current_data, *previous_data;
    long   result;

    if (sampler == NULL || window == 0 || process_fn == NULL) {
        errno = EINVAL;
        return -1;
    }

    pthread_mutex_lock(&sampler->lock);

    current_data   = ring_buffer_peek(sampler->ring_buffer, 0);
    previous_data  = ring_buffer_peek(sampler->ring_buffer, window);

    if (current_data == NULL || previous_data == NULL) {
        pthread_mutex_unlock(&sampler->lock);
        errno = EAGAIN;
        return -1;
    }

    result = process_fn(current_data, previous_data);

    pthread_mutex_unlock(&sampler->lock);

    return result;
}

Sampler* sampler_create_with_subscription_and_start(const unsigned int       interval_ms,
                                                    const size_t             item_size,
                                                    const SamplerCaptureFunc capture_fn,
                                                    const SamplerCallback    callback) {
    Sampler* sampler;

    if (capture_fn == NULL || callback == NULL) {
        errno = EINVAL;
        return NULL;
    }

    sampler = sampler_create(interval_ms, item_size, capture_fn);
    if (sampler == NULL) {
        return NULL;
    }

    if (sampler_subscribe(sampler, callback) != 0) {
        sampler_destroy(sampler);
        return NULL;
    }

    if (sampler_start(sampler) != 0) {
        sampler_destroy(sampler);
        return NULL;
    }

    return sampler;
}