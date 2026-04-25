#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stddef.h>

/**
 * @typedef RingBuffer
 * @brief Opaque handle to a fixed-capacity circular pointer buffer.
 * @details Once the capacity is reached, advancing the head will overwrite the oldest data according to FIFO principles.
 */
typedef struct RingBuffer RingBuffer;

/**
 * @brief Allocates and initialises a new ring buffer.
 * @param capacity The maximum number of elements in the ring buffer before wrapping occurs.
 * @param item_size Memory size of a single element in bytes.
 * @return A pointer to the buffer, or NULL if the heap is exhausted.
 */
RingBuffer* ring_buffer_alloc(size_t capacity, size_t item_size);

/**
 * @brief Releases the buffer memory. Does not free element memory.
 * @param ring_buffer The ring buffer.
 */
void ring_buffer_free(RingBuffer* ring_buffer);

/**
 * @brief Advances the head and returns a pointer to the new current slot.
 * @param ring_buffer The ring buffer.
 * @return The item at the next position.
 */
void* ring_buffer_advance(RingBuffer* ring_buffer);

/**
 * @brief Gets a pointer relative to the current head. 0-indexed.
 * @param ring_buffer The ring buffer.
 * @param offset The offset.
 * @return A pointer to an item relative to the current head.
 */
void* ring_buffer_peek(const RingBuffer* ring_buffer, size_t offset);

/**
 * @brief Gets the capacity of the ring buffer.
 * @param ring_buffer The ring buffer.
 * @return The number of items to fit in the ring buffer.
 */
size_t ring_buffer_get_capacity(const RingBuffer* ring_buffer);

#endif
