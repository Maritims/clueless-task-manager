#ifndef CTM_HASH_MAP_H
#define CTM_HASH_MAP_H
#include "stddef.h"

/**
 * @file hash_map.h
 * @brief A generic hash map and its associated iterator.
 * @author Martin Severin Steffensen
 */

/**
 * @typedef HashFunc
 * @brief User-defined function to generate a hash from a key.
 * @param key Pointer to the key data.
 * @return A size_t hash value.
 * @note For optimal performance, the distribution should be as uniform as possible.
 */
typedef size_t (*HashFunc)(const void* key);

/**
 * @typedef CompareFunc
 * @brief User-defined function to compare two keys for equality.
 * @param a Pointer to the first key.
 * @param b Pointer to the second key.
 * @retval 0 If the keys are equal.
 * @retval Non-zero if the keys are different.
 * @note This is used to resolve collisions; it must be consistent with the HashFunc.
 */
typedef int (*CompareFunc)(const void* a,
                           const void* b);

/**
 * @typedef HashEntry
 * @brief Internal container for a key-value pair.
 * @note Not intended for direct use by the API consumer.
 */
typedef struct HashEntry HashEntry;

/**
 * @typedef HashMap
 * @brief Opaque handle to a HashMap instance.
 * @details Stores elements by value using the sizes provided during creation.
 * Managing the lifecycle of pointers stored within the map is the responsibility of the caller.
 * All access must be performed via hash_map_* functions.
 * Requires hash_map_create() to initialize and hash_map_free() to destroy.
 */
typedef struct HashMap HashMap;

/**
 * @typedef HashMapIter
 * @brief Opaque handle to a hash map iterator instance.
 * @details All access must be performed via hash_map_iter_* functions.
 * Requires hash_map_iter_create() to initialize and hash_map_iter_free() to destroy.
 */
typedef struct HashMapIter HashMapIter;

/**
 * @brief Allocates an empty hash map. Msut be destroyed with hash_map_free().
 * @param key_size The size of the key type used in the map.
 * @param value_size The size of the value type used in the map.
 * @param hash_func The function for hashing keys.
 * @param compare_func The function for comparing keys.
 * @return The new hash map.
 */
HashMap* hash_map_create(size_t      key_size,
                         size_t      value_size,
                         HashFunc    hash_func,
                         CompareFunc compare_func);

/**
 * @brief Releases hash map memory.
 * @param hash_map The hash map.
 */
void hash_map_free(HashMap* hash_map);

/**
 * @brief Gets the number of elements currently stored within the hash map.
 * @param hash_map The hash map.
 * @return The number of elements currently stored within the hash map.
 */
size_t hash_map_count(const HashMap* hash_map);

/**
 * @brief Inserts a key-value pair into the hash map, or updates an existing key-value pair if any entry matches the key.
 * @param hash_map The hash map.
 * @param key The key of the new entry.
 * @param value The value of the new entry.
 * @retval 0 on success.
 * @retval -1 on failure.
 */
int hash_map_put(HashMap*    hash_map,
                 const void* key,
                 const void* value);

/**
 * @brief Gets the value associated iwth the given key.
 * @param hash_map The hash map.
 * @param key The key.
 * @return The value with the given key, or NULL if no such key-value pair exists within the hash map.
 */
void* hash_map_get(const HashMap* hash_map,
                   const void*    key);

/**
 * @brief Removes a key-value pair from the hash map by the pair's key.
 * @param hash_map The hash map.
 * @param key The key.
 * @retval 0 on success.
 * @retval -1 on failure.
 */
int hash_map_remove(HashMap*    hash_map,
                    const void* key);

/**
 * @brief Hashes a key which is guaranteed to be an integer.
 * @param key The key to hash.
 * @return The hashed key.
 */
size_t hash_int(const void* key);

/**
 * @brief Compares two keys guaranteed to be integers.
 * @param a The first key.
 * @param b The second key.
 * @retval 0 when a and b are equal.
 * @retval 1 when a is greater than b.
 * @retval -1 when a is less than b.
 */
int hash_compare_int(const void* a,
                     const void* b);

/**
 * @brief Allocates a new iterator.
 * @param hash_map The hash map.
 * @return The hash map iterator.
 */
HashMapIter* hash_map_iter_create(HashMap* hash_map);

/**
 * @brief Releases iterator memory. Does not release hash map memory.
 * @param iter The hash map iterator.
 */
void hash_map_iter_free(HashMapIter* iter);

/**
 * @brief Gets the key-value pair at the current index and increments the current index.
 * @param iter The hash map iterator.
 * @param key A pointer to where the next key will be stored.
 * @param value A pointer to where the next value will be stored.
 * @return
 */
int hash_map_iter_next(HashMapIter* iter,
                       void**       key,
                       void**       value);

#endif
