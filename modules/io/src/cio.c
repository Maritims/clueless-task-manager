//
// Created by martin on 02.04.2026.
//

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CIO_CHUNK_SIZE 4096

/**
 * Gets the file size for the given file.
 * @param fp File handle
 * @retval 0 If the file is empty, or if it's a virtual file (e.g. /proc/stat).
 * @return The file size
 */
static long cio_get_file_size(FILE* fp) {
    if (fp == NULL) {
        fprintf(stderr, "%s: fp cannot be NULL\n", __func__);
        return -1;
    }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    if (size == 0) {
        char   buffer[CIO_CHUNK_SIZE];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
            if (bytes_read < sizeof(buffer)) {
                size += (long) bytes_read;
            }
        }
    }

    return size;
}

char* cio_read_file(const char* path, size_t* out_size) {
    if (path == NULL) {
        fprintf(stderr, "%s: path cannot be NULL\n", __func__);
        return NULL;
    }
    if (out_size == NULL) {
        fprintf(stderr, "%s: out_size cannot be NULL\n", __func__);
        return NULL;
    }

    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "%s: Failed to open file %s: %s\n", __func__, path, strerror(errno));
        return NULL;
    }

    char* buffer = malloc(CIO_CHUNK_SIZE);
    if (buffer == NULL) {
        fprintf(stderr, "%s: Failed to allocate memory for file %s\n", __func__, path);
        fclose(fp);
        return NULL;
    }

    size_t size       = 0;
    size_t bytes_read = 0;
    size_t capacity   = CIO_CHUNK_SIZE;
    char   chunk[CIO_CHUNK_SIZE];

    while ((bytes_read = fread(chunk, 1, sizeof(chunk), fp)) > 0) {
        // Reallocate once we reach capacity.
        if (size + bytes_read >= capacity) {
            size_t new_capacity = capacity * 2; // Double the capacity.
            while (new_capacity < size + bytes_read) {
                // Keep doubling the capacity until we've got enough capacity.
                new_capacity *= 2;
            }

            char* new_buffer = realloc(buffer, new_capacity);
            if (new_buffer == NULL) {
                fprintf(stderr, "%s: Failed to allocate memory for file %s\n", __func__, path);
                free(buffer);
                fclose(fp);
                return NULL;
            }
            buffer   = new_buffer;
            capacity = new_capacity;
        }

        // Copy the chunk into the main buffer.
        memcpy(buffer + size, chunk, bytes_read);
        size += bytes_read;
    }

    buffer[size] = '\0';

    fclose(fp);
    *out_size = size;
    return buffer;
}
