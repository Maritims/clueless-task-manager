#ifndef ARRAY_H
#define ARRAY_H
#include <stddef.h>

typedef struct Array         Array;
typedef struct ArrayIterator ArrayIterator;

Array* array_create(void);
void   array_free(Array* array);
size_t array_count(const Array* array);
int    array_add(Array* array, void* element);
void*  array_get(const Array* array, size_t index);
int    array_remove(Array* array, size_t index);

ArrayIterator* array_iter_create(Array* array);
void           array_iter_free(ArrayIterator* iter);

/**
 * Gets the array's current index.
 * @param iter The iterator.
 * @param index A pointer to where the current index will be stored.
 * @return An integer representing the outcome of the operation.
 */
int array_iter_get_index(const ArrayIterator* iter, size_t* index);

/**
 * Gets the element at the array's current index and increments the current index. If the index or element arguments are NULL the function should merely increment the internal index and then return.
 * @param iter The iterator.
 * @param index A pointer to where the index of the element will be stored. Can be NULL.
 * @param element A pointer to where the element will be stored. Can be NULL.
 * @return An integer representing the outcome of the operation.
 */
int array_iter_next(ArrayIterator* iter, size_t* index, void** element);

#endif
