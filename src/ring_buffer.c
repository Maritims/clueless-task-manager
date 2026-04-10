#include "ctm/ctm.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct RingBuffer {
    /**
     * The number of items to fit in the ring buffer.
     */
    size_t capacity;
    /**
     * The number of items currently in the buffer.
     */
    size_t count;
    /**
     * Flattened array of bytes.
     */
    char* buffer;
    /**
     * The size of each item in the buffer in bytes.
     */
    size_t item_size;
    /**
     * The index of the most recent item.
     */
    size_t head;
};

RingBuffer* ring_buffer_alloc(const size_t capacity, const size_t item_size) {
    RingBuffer* ring_buffer;

    if ((ring_buffer = malloc(sizeof(RingBuffer))) == NULL) {
        return NULL;
    }

    if ((ring_buffer->buffer = calloc(capacity, item_size)) == NULL) {
        free(ring_buffer);
        return NULL;
    }

    ring_buffer->capacity  = capacity;
    ring_buffer->count     = 0;
    ring_buffer->item_size = item_size;
    ring_buffer->head      = 0;

    return ring_buffer;
}

void ring_buffer_free(RingBuffer* ring_buffer) {
    if (ring_buffer) {
        free(ring_buffer->buffer);
        free(ring_buffer);
    }
}

void* ring_buffer_advance(RingBuffer* ring_buffer) {
    if (ring_buffer->count > 0) {
        ring_buffer->head = (ring_buffer->head + 1) % ring_buffer->capacity;
    }

    if (ring_buffer->count < ring_buffer->capacity) {
        ring_buffer->count++;
    }

    return ring_buffer->buffer + ring_buffer->head * ring_buffer->item_size;
}

void* ring_buffer_peek(const RingBuffer* ring_buffer, const size_t offset) {
    size_t idx;

    if (offset >= ring_buffer->count) {
        errno = EINVAL;
        return NULL;
    }

    idx = (ring_buffer->head + ring_buffer->capacity - offset) % ring_buffer->capacity;

    return (void*)(ring_buffer->buffer + (idx * ring_buffer->item_size));
}

size_t ring_buffer_get_capacity(const RingBuffer* ring_buffer) {
    if (ring_buffer == NULL) {
        errno = EINVAL;
        return 0;
    }
    return ring_buffer->capacity;
}