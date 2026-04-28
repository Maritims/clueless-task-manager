#include "hash_map.h"
#include "test.h"

TEST(create)
{
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

TEST(put)
{
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

TEST(get)
{
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

TEST(put_should_update_existing_entry)
{
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

TEST(remove)
{
    /* arrange */
    int      success;
    int      key;
    HashMap* map;

    map = hash_map_create(sizeof(int), sizeof(int), hash_int, hash_compare_int);
    key = 1337;
    hash_map_put(map, &key, "foo");

    /* act */
    hash_map_remove(map, &key);

    /* assert */
    success = assert_unsigned_long_equality(0, hash_map_count(map), "hash_map_count(map) should return 0");

    hash_map_free(map);
    return success;
}

TEST(count_when_map_is_empty)
{
    /* arrange */
    int      success;
    HashMap* map;
    size_t   actual;

    map = hash_map_create(sizeof(int), sizeof(int), hash_int, hash_compare_int);

    /* act */
    actual = hash_map_count(map);

    /* assert */
    success = assert_unsigned_long_equality(0, actual, "hash_map_count() should return 0");

    hash_map_free(map);
    return success;
}

TEST(count_when_map_has_elements)
{
    /* arrange */
    int      success;
    HashMap* map;
    size_t   actual;
    int      key;

    map = hash_map_create(sizeof(int), sizeof(int), hash_int, hash_compare_int);
    key = 1337;
    hash_map_put(map, &key, "foo");

    /* act */
    actual = hash_map_count(map);

    /* assert */
    success = assert_unsigned_long_equality(1, actual, "hash_map_count() should return 1");

    hash_map_free(map);
    return success;
}

TEST(iter_create)
{
    /* arrange */
    int          success;
    HashMap*     map;
    HashMapIter* iter;

    map  = hash_map_create(sizeof(int), sizeof(int), hash_int, hash_compare_int);
    iter = hash_map_iter_create(map);

    /* act */
    success = assert_not_null(iter, "hash_map_iter_create() should not return NULL");

    hash_map_iter_free(iter);
    hash_map_free(map);
    return success;
}

TEST(iter_next)
{
    /* arrange */
    int          success;
    HashMap*     map;
    HashMapIter* iter;
    int          first_key, second_key;
    int*         actual_key;
    void*        actual_value;

    map        = hash_map_create(sizeof(int), sizeof(int), hash_int, hash_compare_int);
    iter       = hash_map_iter_create(map);
    first_key  = 1337;
    second_key = 42;
    hash_map_put(map, &first_key, "foo");
    hash_map_put(map, &second_key, "bar");

    /* act */
    hash_map_iter_next(iter, (void **) &actual_key, &actual_value);

    /* assert */
    success = assert_int_equality(1337, *actual_key, "hash_map_iter_next() should yield the key of the first element");
    if (success == 0) success = assert_string_equality("foo", actual_value, "hash_map_iter_next() should yield the value of the first element");

    hash_map_iter_free(iter);
    hash_map_free(map);
    return success;
}

#define TESTS \
    TEST_CASE(create)\
    TEST_CASE(put)\
    TEST_CASE(get)\
    TEST_CASE(remove)\
    TEST_CASE(put_should_update_existing_entry)\
    TEST_CASE(count_when_map_is_empty)\
    TEST_CASE(count_when_map_has_elements)\
    TEST_CASE(iter_create)\
    TEST_CASE(iter_next)

RUN_TEST_SUITE(TESTS)
