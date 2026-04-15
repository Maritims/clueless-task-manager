#include <errno.h>
#include <stdlib.h>

#include "collections/array.h"

struct ArrayIterator {
    Array* array;
    size_t index;
};

ArrayIterator* array_iter_create(Array* array) {
    ArrayIterator* iter;

    if (array == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if ((iter = malloc(sizeof(ArrayIterator))) == NULL) {
        return NULL;
    }

    iter->array = array;
    iter->index = 0;

    return iter;
}

void array_iter_free(ArrayIterator* iter) {
    if (iter) {
        free(iter);
    }
}

int array_iter_get_index(const ArrayIterator* iter, size_t* index) {
    if (iter == NULL || index == NULL) {
        errno = EINVAL;
        return -1;
    }
    *index = iter->index;
    return 0;
}

int array_iter_next(ArrayIterator* iter, size_t* index, void** element) {
    if (iter == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (iter->index >= array_count(iter->array)) {
        errno = ERANGE;
        return -1;
    }

    if (index == NULL) {
        iter->index++;
        return 0;
    }

    *index = iter->index++;

    if (element != NULL) {
        *element = array_get(iter->array, *index);
    }

    return 0;
}
