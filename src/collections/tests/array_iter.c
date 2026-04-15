#include "test.h"
#include "collections/array.h"

int test_create(void) {
    /* arrange */
    int            success;
    Array*         array;
    ArrayIterator* actual;

    array = array_create();


    /* act */
    actual = array_iter_create(array);

    /* assert */
    success = assert_not_null(actual, "array_iter_create() should not return NULL");

    array_iter_free(actual);
    array_free(array);
    return success;
}

int test_next(void) {
    /* arrange */
    int            success, a, b;
    int*           actual;
    size_t         actual_index;
    Array*         array;
    ArrayIterator* iter;

    array = array_create();
    iter  = array_iter_create(array);
    a     = 1337;
    b     = 31337;
    array_add(array, &a);
    array_add(array, &b);

    /* act */
    array_iter_next(iter, &actual_index, (void **) &actual);

    /* assert */
    success = assert_unsigned_long_equality(0, actual_index, "array_iter_next() should return the index of the returned element");
    if (success == 0) success = assert_not_null(actual, "array_iter_next() should return a pointer to the returned element");
    if (success == 0) success = assert_int_equality(1337, *actual, "array_iter_next() should return the value of the returned element");

    array_free(array);
    array_iter_free(iter);
    return success;
}

int test_get_index(void) {
    /* arrange */
    int            success;
    size_t         actual;
    Array*         array;
    ArrayIterator* iter;

    array = array_create();
    iter  = array_iter_create(array);

    /* act */
    array_iter_get_index(iter, &actual);

    /* assert */
    success = assert_unsigned_long_equality(0, actual, "array_iter_get_index() should return the index of the current element");

    array_free(array);
    array_iter_free(iter);
    return success;
}

int test_get_index_after_next(void) {
    /* arrange */
    int            success;
    size_t         actual;
    Array*         array;
    ArrayIterator* iter;
    int            a;

    array = array_create();
    iter  = array_iter_create(array);
    a     = 1337;
    array_add(array, &a);

    /* act */
    array_iter_next(iter, NULL, NULL);
    array_iter_get_index(iter, &actual);

    /* assert */
    success = assert_unsigned_long_equality(1, actual, "array_iter_get_index() should return the index of the current element");

    array_free(array);
    array_iter_free(iter);
    return success;
}

#define TESTS\
    TEST_CASE(create)\
    TEST_CASE(next)\
    TEST_CASE(get_index)\
    TEST_CASE(get_index_after_next)
RUN_TEST_SUITE(TESTS)
