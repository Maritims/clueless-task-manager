#ifndef CTM_ARRAY_H
#define CTM_ARRAY_H

#ifndef INSIDE_CTM_COLLECTIONS_H
#error "Only collections/collections.h can be included directly!"
#endif
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
int            array_iter_next(ArrayIterator* iter, size_t* index, void** element);

#endif
