#ifndef CTM_RING_BUFFER_H
#define CTM_RING_BUFFER_H

#ifndef INSIDE_CTM_H
#error "Only ctm/ctm.h can be included directly!"
#endif

#include <stddef.h>

typedef struct RingBuffer RingBuffer;

/**
 * Allocates and initialises a new ring buffer.
 * @param capacity The number of items to fit in the ring buffer.
 * @param item_size The size of each item in bytes.
 * @return The allocated ring buffer, or NULL if allocation fails.
 */
RingBuffer* ring_buffer_alloc(size_t capacity, size_t item_size);

/**
 * Frees the memory associated with the ring buffer.
 * @param ring_buffer The ring buffer.
 */
void ring_buffer_free(RingBuffer* ring_buffer);

/**
 * Advances the head and returns a pointer to the new current slot.
 * @param ring_buffer The ring buffer.
 * @return The item at the next position.
 */
void* ring_buffer_advance(RingBuffer* ring_buffer);

/**
 * Gets a pointer relative to the current head. 0-indexed.
 * @param ring_buffer The ring buffer.
 * @param offset The offset.
 * @return A pointer to an item relative to the current head.
 */
void* ring_buffer_peek(const RingBuffer* ring_buffer, size_t offset);

/**
 * Gets the capacity of the ring buffer.
 * @param ring_buffer The ring buffer.
 * @return The number of items to fit in the ring buffer.
 */
size_t ring_buffer_get_capacity(const RingBuffer* ring_buffer);

#endif
