#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem.h"
#include "platform.h"
#include "internal/mem_internal.h"

const char* mem_strerror(const mem_result_t result)
{
    switch (result) {
        case MEM_SUCCESS:
            return "Success";
        case MEM_ERR_INVALID_ARG:
            return "Invalid argument";
        case MEM_ERR_OPEN:
            return "Failed to open file";
        case MEM_ERR_READ:
            return "Failed to read file";
        case MEM_ERR_PARSE:
            return "Failed to parse file";
        case MEM_ERR_INTERNAL:
            return "Internal error";
        default:
            return "Unknown error";
    }
}

mem_result_t mem_read(mem_t* out)
{
    FILE*  fp;
    size_t bytes_read;
    char   buffer[1024];
    char*  line_buffer;
    char*  save_ptr;
    int    read_available = -1, read_free = -1, read_total = -1;

    if (out == NULL) {
        return MEM_ERR_INVALID_ARG;
    };
    if ((fp = fopen("/proc/meminfo", "r")) == NULL) {
        return MEM_ERR_OPEN;
    }

    bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    if (bytes_read <= 0) {
        return MEM_ERR_READ;
    }

    line_buffer = ctm_strtok_r(buffer, "\n", &save_ptr);
    while (line_buffer != NULL) {
        if (strncmp(line_buffer, "MemAvailable:", 13) == 0) {
            out->mem_available = strtoul(line_buffer + 13, NULL, 10);
            read_available     = 1;
        } else if (strncmp(line_buffer, "MemFree:", 8) == 0) {
            out->mem_free = strtoul(line_buffer + 8, NULL, 10);
            read_free     = 1;
        } else if (strncmp(line_buffer, "MemTotal:", 9) == 0) {
            out->mem_total = strtoul(line_buffer + 9, NULL, 10);
            read_total     = 1;
        }
        line_buffer = ctm_strtok_r(NULL, "\n", &save_ptr);
    }

    if (read_available == -1 || read_free == -1 || read_total == -1) {
        return MEM_ERR_PARSE;
    }

    out->mem_available = 0;
    out->mem_free      = 0;
    out->mem_total     = 0;

    return MEM_SUCCESS;
}

mem_result_t mem_destroy(mem_t* mem)
{
    if (mem) {
        free(mem);
        return MEM_SUCCESS;
    }
    return MEM_ERR_INVALID_ARG;
}

mem_result_t mem_total(const mem_t*   mem,
                       unsigned long* out)
{
    if (mem && out) {
        *out = mem->mem_total;
        return MEM_SUCCESS;
    }
    return MEM_ERR_INVALID_ARG;
}

mem_result_t mem_free(const mem_t*   mem,
                      unsigned long* out)
{
    if (mem && out) {
        *out = mem->mem_free;
        return MEM_SUCCESS;
    }
    return MEM_ERR_INVALID_ARG;
}

mem_result_t mem_available(const mem_t*   mem,
                           unsigned long* out)
{
    if (mem && out) {
        *out = mem->mem_available;
        return MEM_SUCCESS;
    }
    return MEM_ERR_INVALID_ARG;
}
