#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "collections/array.h"

#include <stdio.h>

/**
 * @brief The minimum capacity for newly initialized arrays.
 */
#define MINIMUM_CAPACITY 100

/**
 * @struct Array
 * @brief Internal representation of the dynamic array.
 * * The array grows exponentially by a factor of 2 when capacity is reached.
 */
struct Array {
    size_t count;    /**< Number of elements currently in use. */
    size_t capacity; /**< Total allocated slots in the data buffer. */
    void** data;     /**< The data buffer. */
};

Array* array_create(void) {
    Array* array;

    if ((array = malloc(sizeof(Array))) == NULL) {
        return NULL;
    }

    array->count    = 0;
    array->capacity = MINIMUM_CAPACITY;
    if ((array->data = calloc(array->capacity, sizeof(void *))) == NULL) {
        free(array);
        return NULL;
    }

    return array;
}

void array_free(Array* array) {
    if (array) {
        free(array->data);
        free(array);
    }
}

size_t array_count(const Array* array) {
    if (array == NULL) {
        fprintf(stderr, "array_count: array is NULL\n");
        errno = EINVAL;
        return 0;
    }
    return array->count;
}

int array_add(Array* array, void* element) {
    if (array == NULL || element == NULL) {
        fprintf(stderr, "array_add: array or element is NULL\n");
        errno = EINVAL;
        return -1;
    }
    if (array->count >= array->capacity) {
        size_t new_capacity;
        void** new_data;

        new_capacity = array->capacity * 2;
        new_data     = realloc(array->data, new_capacity * sizeof(void *));
        if (new_data == NULL) {
            return -1;
        }

        array->capacity = new_capacity;
        array->data     = new_data;
    }
    array->data[array->count++] = element;
    return 0;
}

void* array_get(const Array* array, const size_t index) {
    if (array == NULL || index >= array->count) {
        errno = EINVAL;
        return NULL;
    }
    return array->data[index];
}

int array_remove(Array* array, const size_t index) {
    if (array == NULL || index >= array->count) {
        errno = EINVAL;
        return -1;
    }

    if (index < array->count - 1) {
        memmove(&array->data[index], &array->data[index + 1], (array->count - index - 1) * sizeof(void *));
    }

    array->count--;
    array->data[array->count] = NULL;
    return 0;
}
