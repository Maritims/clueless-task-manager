#include "../../../include/array.h"

#include <stdio.h>

#include "test.h"

int test_create(void) {
    /* arrange */
    int    success;
    Array* actual;

    /* act */
    actual = array_create();

    /* assert */
    success = assert_not_null(actual, "array_create() should not return NULL");

    array_free(actual);
    return success;
}

int test_add_and_count(void) {
    /* arrange */
    int    success;
    Array* array;
    int    i;
    size_t actual;

    /* act */
    i     = 1337;
    array = array_create();
    array_add(array, &i);
    actual = array_count(array);

    /* assert */
    success = assert_unsigned_long_equality(1, actual, "array_count() should return the number of elements in the array");

    array_free(array);
    return success;
}

int test_add_and_get(void) {
    /* arrange */
    int success;
    Array* array;
    int i;
    int actual;

    /* act */
    i = 1337;
    array = array_create();
    array_add(array, &i);
    actual = *(int *) array_get(array, 0);

    /* assert */
    success = assert_int_equality(i, actual, "array_get(0) should return the value of the first element");

    array_free(array);
    return success;
}

int test_add_and_remove(void) {
    /* arrange */
    int success, element;
    Array* array;

    array = array_create();
    array_add(array, &element);

    /* act */
    success = array_remove(array, 0);

    /* assert */
    success = assert_int_equality(0, success, "array_remove(0) should return 0");
    if (success == 0) success = assert_unsigned_long_equality(0, array_count(array), "array_count() should return 0");

    array_free(array);
    return success;
}

#define TESTS \
    TEST_CASE(create)\
    TEST_CASE(add_and_count)\
    TEST_CASE(add_and_get)\
    TEST_CASE(add_and_remove)

RUN_TEST_SUITE(TESTS)