#include <stdio.h>
#include "ui.h"

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

int main(int argc, char** argv) {
    return ui_start(argc, argv);
}
