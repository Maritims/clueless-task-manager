#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "ctm/ctm.h"
#include "ctm/sampler.h"
#include "ctm/ui.h"

int ui_update(void* user_data) {
    Sampler* sampler;
    long     usage;

    if (user_data == NULL) {
        errno = EINVAL;
        return -1;
    }

    sampler = SAMPLER(user_data);

    if ((usage = sampler_get_value(sampler, 10, (SamplerProcessFunc) cpu_get_total_usage)) >= 0) {
        printf("\rCPU usage: %Lg%%    ", (long double) usage / 1000.0);
        fflush(stdout);
        return 0;
    }

    if (errno == EAGAIN) {
        /* Not enough samples yet */
        return 0;
    }

    fprintf(stderr, "\nui_update error: %s\n", strerror(errno));
    return -1;
}

int ui_start(int argc, char** argv) {
    Sampler* cpu_sampler;

    (void) argc;
    (void) argv;

    cpu_sampler = sampler_create_with_subscription_and_start(500,
                                                             cpu_size(),
                                                             (SamplerCaptureFunc) cpu_capture,
                                                             (SamplerCallback) ui_update);
    if (cpu_sampler == NULL) {
        fprintf(stderr, "Failed to create and start CPU sampler: %s\n", strerror(errno));
        return -1;
    }

    printf("Press any key to continue\n");

    getchar();

    printf("\nShutting down...\n");
    sampler_destroy(cpu_sampler);
    return 0;
}
