#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "collections/array.h"

#define MINIMUM_CAPACITY 100

struct Array {
    size_t count;
    size_t capacity;
    size_t element_size;
    void** data;
};

Array* array_create(void) {
    Array* array;

    if ((array = malloc(sizeof(Array))) == NULL) {
        return NULL;
    }

    array->count    = 0;
    array->capacity = MINIMUM_CAPACITY;
    if ((array->data = calloc(array->capacity, sizeof(void*))) == NULL) {
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
        errno = EINVAL;
        return 0;
    }
    return array->count;
}

int array_add(Array* array, void* element) {
    if (array == NULL || element == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (array->count >= array->capacity) {
        size_t new_capacity;
        void** new_data;

        new_capacity = array->capacity * 2;
        new_data     = realloc(array->data, new_capacity * sizeof(void*));
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
        memmove(&array->data[index], &array->data[index + 1], (array->count - index - 1) * sizeof(void*));
    }

    array->count--;
    array->data[array->count] = NULL;
    return 0;
}
