#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "ctm/ctm.h"

void cui_format_duration(const unsigned long time_in_ms, char* buffer, const size_t buffer_size) {
    unsigned int hours, minutes, seconds;

    if (buffer == NULL) {
        fprintf(stderr, "ctm_format_duration: buffer cannot be NULL\n");
        return;
    }
    if (buffer_size == 0) {
        fprintf(stderr, "ctm_format_duration: buffer_size cannot be 0\n");
        return;
    }

    hours   = time_in_ms == 0 ? 0 : (int) (time_in_ms / 3600000);
    minutes = time_in_ms == 0 ? 0 : (int) (time_in_ms % 3600000 / 60000);
    seconds = time_in_ms == 0 ? 0 : (int) (time_in_ms % 60000 / 1000);

    snprintf(buffer, buffer_size, "%02u:%02u:%02u", hours, minutes, seconds);
    buffer[buffer_size - 1] = '\0';
}

/* endregion */

int main(int argc, char** argv) {
    size_t      i;
    CPUSampler* sampler = cpu_sampler_create(1000);
    if (sampler == NULL) {
        fprintf(stderr, "Failed to create CPU sampler\n");
        return -1;
    }

    (void) argc;
    (void) argv;

    if (cpu_sampler_start(sampler) != 0) {
        fprintf(stderr, "Failed to start CPU sampler\n");
        cpu_sampler_destroy(sampler);
        return -1;
    }

    printf("Monitoring CPU usage. Press Ctrl+C to stop.\n");
    printf("Waiting for history to populate...\n");

    for (i = 0; i < 10; i++) {
        long usage;
        sleep(1);

        usage = cpu_sampler_get_avg_usage(sampler, 5);

        if (usage >= 0) {
            printf("Current Load (5s avg): %5.2Lg%%\n", usage / (long double) 100.0);
        } else {
            printf("Calculating... (need more samples)\n");
        }
    }

    printf("\nShutting down...\n");
    cpu_sampler_destroy(sampler);
    return 0;
}
