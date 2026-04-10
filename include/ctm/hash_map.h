#ifndef CTM_HASH_MAP_H
#define CTM_HASH_MAP_H
#include "stddef.h"

#ifndef INSIDE_CTM_H
#error "Only ctm/ctm.h can be included directly!"
#endif

typedef size_t (*          HashFunc)(const void* key);
typedef int (*             CompareFunc)(const void* a, const void* b);
typedef struct HashEntry   HashEntry;
typedef struct HashMap     HashMap;
typedef struct HashMapIter HashMapIter;

HashMap* hash_map_create(size_t key_size, size_t value_size, HashFunc hash_func, CompareFunc compare_func);
void     hash_map_free(HashMap* hash_map);
size_t   hash_map_count(const HashMap* hash_map);
int      hash_map_put(HashMap* hash_map, const void* key, const void* value);
void*    hash_map_get(const HashMap* hash_map, const void* key);

/* hash functions */
size_t hash_int(const void* key);
int    hash_compare_int(const void* a, const void* b);

/* hashmap iterator */
HashMapIter* hash_map_iter_create(HashMap* hash_map);
void         hash_map_iter_free(HashMapIter* iter);
int          hash_map_iter_next(HashMapIter* iter, void** key, void** value);

#endif
