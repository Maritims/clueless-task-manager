#include "ctm/ctm.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct HashEntry {
    void* key;
    void* value;
};

struct HashMap {
    HashEntry*  entries;
    size_t      capacity;
    size_t      count;
    size_t      key_size;
    size_t      value_size;
    HashFunc    hash_func;
    CompareFunc compare_func;
};

HashMap* hash_map_create(const size_t key_size, const size_t value_size, const HashFunc hash_func, const CompareFunc compare_func) {
    HashMap* hash_map;

    if (key_size == 0 || value_size == 0 || hash_func == NULL || compare_func == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if ((hash_map = malloc(sizeof(HashMap))) == NULL) {
        return NULL;
    }

    hash_map->capacity     = 16;
    hash_map->count        = 0;
    hash_map->key_size     = key_size;
    hash_map->value_size   = value_size;
    hash_map->hash_func    = hash_func;
    hash_map->compare_func = compare_func;

    if ((hash_map->entries = calloc(hash_map->capacity, sizeof(HashEntry))) == NULL) {
        free(hash_map);
        return NULL;
    }

    return hash_map;
}

void hash_map_free(HashMap* hash_map) {
    size_t i;

    if (hash_map == NULL) {
        return;
    }

    for (i = 0; i < hash_map->capacity; i++) {
        if (hash_map->entries[i].key != NULL) {
            free(hash_map->entries[i].key);
            free(hash_map->entries[i].value);
        }
    }

    free(hash_map->entries);
    free(hash_map);
}

size_t hash_map_count(const HashMap* hash_map) {
    if (hash_map == NULL) {
        errno = EINVAL;
        return 0;
    }
    return hash_map->count;
}

static int hash_map_resize(HashMap* hash_map) {
    size_t     old_capacity, new_capacity, i, index;
    HashEntry *old_entries, *new_entries;

    if (hash_map == NULL) {
        errno = EINVAL;
        return -1;
    }

    old_capacity = hash_map->capacity;
    new_capacity = old_capacity * 2;
    old_entries  = hash_map->entries;

    new_entries = calloc(new_capacity, sizeof(HashEntry));
    if (new_entries == NULL) {
        errno = ENOMEM;
        return -1;
    }

    for (i = 0; i < old_capacity; i++) {
        if (old_entries[i].key != NULL) {
            index = hash_map->hash_func(old_entries[i].key) % new_capacity;

            while (new_entries[index].key != NULL) {
                index = (index + 1) % new_capacity;
            }

            new_entries[index].key   = old_entries[i].key;
            new_entries[index].value = old_entries[i].value;
        }
    }

    free(old_entries);
    hash_map->entries  = new_entries;
    hash_map->capacity = new_capacity;
    return 0;
}

int hash_map_put(HashMap* hash_map, const void* key, const void* value) {
    size_t index;

    if (hash_map == NULL || key == NULL || value == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (hash_map->count >= hash_map->capacity * 0.75l) {
        if (hash_map_resize(hash_map) != 0) {
            return -1;
        }
    }

    index = hash_map->hash_func(key) % hash_map->capacity;

    while (hash_map->entries[index].key != NULL) {
        if (hash_map->compare_func(key, hash_map->entries[index].key) == 0) {
            memcpy(hash_map->entries[index].value, value, hash_map->value_size);
            return 0;
        }
        index = (index + 1) % hash_map->capacity;
    }

    hash_map->entries[index].key = malloc(hash_map->key_size);
    if (hash_map->entries[index].key == NULL) {
        return -1;
    }

    hash_map->entries[index].value = malloc(hash_map->value_size);
    if (hash_map->entries[index].value == NULL) {
        free(hash_map->entries[index].key);
        hash_map->entries[index].key = NULL;
        return -1;
    }

    memcpy(hash_map->entries[index].key, key, hash_map->key_size);
    memcpy(hash_map->entries[index].value, value, hash_map->value_size);
    hash_map->count++;
    return 0;
}

void* hash_map_get(const HashMap* hash_map, const void* key) {
    size_t index;
    if (hash_map == NULL || key == NULL) {
        errno = EINVAL;
        return NULL;
    }
    index = hash_map->hash_func(key) % hash_map->capacity;
    while (hash_map->entries[index].key != NULL) {
        if (hash_map->compare_func(key, hash_map->entries[index].key) == 0) {
            return hash_map->entries[index].value;
        }
        index = (index + 1) % hash_map->capacity;
    }
    return NULL;
}

struct HashMapIter {
    HashMap* hash_map;
    size_t   index;
};

size_t hash_int(const void* key) {
    /* Cast the void pointer back to an int pointer and dereference */
    const int pid = *(const int*)key;

    /* A simple multiplicative hash to distribute PIDs better across buckets */
    return (size_t)pid * 2654435761U;
}

int hash_compare_int(const void* a, const void* b) {
    const int pid_a = *(const int*)a;
    const int pid_b = *(const int*)b;

    return (pid_a == pid_b) ? 0 : (pid_a < pid_b ? -1 : 1);
}

HashMapIter* hash_map_iter_create(HashMap* hash_map) {
    HashMapIter* iter;

    if (hash_map == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if ((iter = malloc(sizeof(HashMapIter))) == NULL) {
        return NULL;
    }

    iter->hash_map = hash_map;
    iter->index    = 0;
    return iter;
}

void hash_map_iter_free(HashMapIter* hash_map_iter) {
    if (hash_map_iter) {
        hash_map_free(hash_map_iter->hash_map);
        free(hash_map_iter);
    }
}

int hash_map_iter_next(HashMapIter* iter, void** key, void** value) {
    if (iter == NULL || key == NULL || value == NULL) {
        errno = EINVAL;
        return -1;
    }

    while (iter->index < iter->hash_map->capacity) {
        const size_t i = iter->index++;

        if (iter->hash_map->entries[i].key != NULL) {
            *key   = iter->hash_map->entries[i].key;
            *value = iter->hash_map->entries[i].value;
            return 0;
        }
    }

    return -1;
}
