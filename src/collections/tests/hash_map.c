#include "../include/collections/hash_map.h"
#include "test.h"

int test_create(void) {
    /* arrange */
    int      success;
    HashMap* actual;

    /* act */
    actual = hash_map_create(sizeof(int), sizeof(int), hash_int, hash_compare_int);

    /* assert */
    success = assert_not_null(actual, "hash_map_create() should not return NULL");

    hash_map_free(actual);
    return success;
}

int test_put(void) {
    /* arrange */
    int      success;
    int      key;
    HashMap* map;

    map = hash_map_create(sizeof(int), sizeof(int), hash_int, hash_compare_int);
    key = 1337;

    /* act */
    success = hash_map_put(map, &key, "foo");

    /* assert */
    success = assert_int_equality(0, success, "hash_map_put() should return 0");

    hash_map_free(map);
    return success;
}

int test_get(void) {
    /* arrange */
    int      success;
    int      key;
    void*    value;
    HashMap* map;

    map = hash_map_create(sizeof(int), sizeof(int), hash_int, hash_compare_int);
    key = 1337;
    hash_map_put(map, &key, "foo");

    /* act */
    value = hash_map_get(map, &key);

    /* assert */
    success = assert_not_null(value, "hash_map_get(1337) should not return NULL");
    if (success == 0) success = assert_string_equality("foo", value, "hash_map_get(1337) should return \"foo\"");

    hash_map_free(map);
    return success;
}

int test_put_should_update_existing_entry(void) {
    /* arrange */
    int      success;
    int      key;
    void*    value;
    HashMap* map;

    map = hash_map_create(sizeof(int), sizeof(int), hash_int, hash_compare_int);
    key = 1337;
    hash_map_put(map, &key, "foo");
    hash_map_put(map, &key, "bar");

    /* act */
    value = hash_map_get(map, &key);

    /* assert */
    success = assert_string_equality("bar", value, "hash_map_get(1337) should return \"bar\"");

    hash_map_free(map);
    return success;
}

int main(void) {
    const TestCase test_cases[] = {
        {"create", test_create},
        {"put", test_put},
        {"get", test_get},
        {"put_should_update_existing_entry", test_put_should_update_existing_entry}
    };
    const size_t test_count = sizeof(test_cases) / sizeof(test_cases[0]);
    return run_all_tests(test_cases, test_count);
}
