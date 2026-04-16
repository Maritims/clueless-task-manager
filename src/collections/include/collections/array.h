#ifndef ARRAY_H
#define ARRAY_H
#include <stddef.h>

/**
 * @file array.h
 * @brief A generic dynamic array and its associated iterator.
 * @author Martin Severin Steffensen
 */

/**
 * @typedef Array
 * @brief Opaque handle to a dynamic array instance.
 * @details All access must be performed via array_* functions.
 * Requires array_create() to initialize and array_free() to destroy.
 */
typedef struct Array Array;

/**
 * @typedef ArrayIterator
 * @brief Opaque handle to an array iterator instance.
 * @details All access must be performed via array_iter_* functions.
 * Requires array_iter_create() to initialize and array_iter_free() to destroy.
 */
typedef struct ArrayIterator ArrayIterator;

/**
 * @brief Allocates an empty array. Must be destroyed with array_free().
 * @return The new array.
 */
Array* array_create(void);

/**
 * @brief Releases array memory. Does not free the elements themselves.
 * @param array The array.
 */
void array_free(Array* array);

/**
 * @brief Gets the number of elements currently stored within the array.
 * @param array The array.
 * @return The number of elements currently stored within the array.
 */
size_t array_count(const Array* array);

/**
 * @brief Adds an element to the array.
 * @param array The array.
 * @param element The element.
 * @retval 0 on success.
 * @retval -1 on failure.
 */
int array_add(Array* array, void* element);

/**
 * @brief Gets an element from the array by the element's index.
 * @param array The array.
 * @param index The element index.
 * @return The element at the index, or NULL if the index is out of bounds.
 */
void* array_get(const Array* array, size_t index);

/**
 * @brief Removes an element from array by the element's index.
 * @param array The array.
 * @param index The element index.
 * @retval 0 on success.
 * @retval -1 on failure.
 */
int array_remove(Array* array, size_t index);

/**
 * @brief Allocates a new iterator.
 * @param array The array.
 * @return The array iterator.
 */
ArrayIterator* array_iter_create(Array* array);

/**
 * @brief Releases iterator memory. Does not release array memory.
 * @param iter The array iterator.
 */
void array_iter_free(ArrayIterator* iter);

/**
 * @brief Gets the array's current index.
 * @param iter The iterator.
 * @param index A pointer to where the current index will be stored.
 * @retval 0 on success.
 * @retval -1 on failure.
 */
int array_iter_get_index(const ArrayIterator* iter, size_t* index);

/**
 * @brief Gets the element at the array's current index and increments the current index.
 * @details If the index or element arguments are NULL the function should merely increment the internal index and then return.
 * @param iter The iterator.
 * @param index A pointer to where the index of the element will be stored. Can be NULL.
 * @param element A pointer to where the element will be stored. Can be NULL.
 * @retval 0 on success.
 * @retval -1 on failure.
 */
int array_iter_next(ArrayIterator* iter, size_t* index, void** element);

#endif
