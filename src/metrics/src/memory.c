#include "metrics/memory.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Memory {
    unsigned long mem_total;
    unsigned long mem_available;
    unsigned long mem_free;
};

Memory* memory_get(void) {
    Memory* memory;
    FILE*   fp;
    char    buffer[1024];
    size_t  bytes_read;
    char*   line_buffer;
    char*   save_ptr;
    int     read_available = -1, read_free = -1, read_total = -1;

    memory = malloc(sizeof(Memory));
    if (!memory) {
        fprintf(stderr, "memory_get: Failed to allocate memory for memory stats: %d\n", errno);
        return NULL;
    }

    fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        fprintf(stderr, "memory_get: Failed to open /proc/meminfo: %s\n", strerror(errno));
        return NULL;
    }

    bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    if (bytes_read <= 0) {
        fprintf(stderr, "memory_get: Failed to read /proc/meminfo: %s\n", strerror(errno));
        return NULL;
    }

    line_buffer = strtok_r(buffer, "\n", &save_ptr);
    while (line_buffer != NULL) {
        if (strncmp(line_buffer, "MemAvailable:", 13) == 0) {
            memory->mem_available = strtoul(line_buffer + 13, NULL, 10);
            read_available        = 1;
        } else if (strncmp(line_buffer, "MemFree:", 8) == 0) {
            memory->mem_free = strtoul(line_buffer + 8, NULL, 10);
            read_free        = 1;
        } else if (strncmp(line_buffer, "MemTotal:", 9) == 0) {
            memory->mem_total = strtoul(line_buffer + 9, NULL, 10);
            read_total        = 1;
        }
        line_buffer = strtok_r(NULL, "\n", &save_ptr);
    }

    if (read_available == -1 || read_free == -1 || read_total == -1) {
        fprintf(stderr, "memory_get: Failed to parse /proc/meminfo: %s\n", strerror(errno));
        memory_free(memory);
        return NULL;
    }

    memory->mem_available = 0;
    memory->mem_free      = 0;
    memory->mem_total     = 0;

    return memory;
}

void memory_free(Memory* memory) {
    if (memory) {
        free(memory);
    }
}

unsigned long memory_get_total(const Memory* memory) {
    if (!memory) {
        fprintf(stderr, "memory_get_total: Invalid argument: NULL\n");
        return -1;
    }
    return memory->mem_total;
}

unsigned long memory_get_free(const Memory* memory) {
    if (!memory) {
        fprintf(stderr, "memory_get_free: Invalid argument: NULL\n");
        return -1;
    }
    return memory->mem_free;
}

unsigned long memory_get_available(const Memory* memory) {
    if (!memory) {
        fprintf(stderr, "memory_get_available: Invalid argument: NULL\n");
        return -1;
    }
    return memory->mem_available;
}
