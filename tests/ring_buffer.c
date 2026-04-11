#include "../include/collections/ring_buffer.h"

#include <errno.h>

#include "test.h"

int test_alloc(void) {
    /* arrange */
    int         success;
    RingBuffer* actual;

    /* act */
    actual = ring_buffer_alloc(10, sizeof(int));

    /* assert */
    success = assert_not_null(actual, "ring_buffer_alloc() should not return NULL");
    if (success == 0) success = assert_null(ring_buffer_peek(actual, 0), "ring_buffer_peek() should return NULL when buffer is empty");
    if (success == 0) success = assert_int_equality(EINVAL, errno, "ring_buffer_peek() should set errno to EINVAL when buffer is empty");

    ring_buffer_free(actual);
    return success;
}

int test_advance_and_peek(void) {
    /* arrange */
    int         success;
    RingBuffer* ring_buffer;
    int*        buffer_item;

    ring_buffer = ring_buffer_alloc(3, sizeof(int));

    /* First advance */
    buffer_item  = ring_buffer_advance(ring_buffer);
    *buffer_item = 1337;
    success      = assert_int_equality(1337, *(int *) ring_buffer_peek(ring_buffer, 0), "ring_buffer_peek() should return the item at the head of the buffer");
    if (success == 0) success = assert_null(ring_buffer_peek(ring_buffer, 1), "ring_buffer_peek() with offset = 1 should return NULL when count is 1");
    if (success != 0) return success;

    /* Second advance */
    buffer_item  = ring_buffer_advance(ring_buffer);
    *buffer_item = 42;
    success      = assert_int_equality(42, *(int *) ring_buffer_peek(ring_buffer, 0), "ring_buffer_peek() should return the item at the head of the buffer");
    if (success == 0) success = assert_int_equality(1337, *(int *) ring_buffer_peek(ring_buffer, 1), "ring_buffer_peek() with offset = 1 should return 1337 which is at the end of the buffer");

    ring_buffer_free(ring_buffer);
    return success;
}

int test_wrap_around(void) {
    /* arrange */
    int         success;
    RingBuffer* ring_buffer;
    int *       v1, *v2, *v3;

    ring_buffer = ring_buffer_alloc(2, sizeof(int));

    /* advance twice */
    v1      = (int *) ring_buffer_advance(ring_buffer);
    *v1     = 10;
    v2      = (int *) ring_buffer_advance(ring_buffer);
    *v2     = 20;
    success = assert_int_equality(20, *(int *) ring_buffer_peek(ring_buffer, 0), "ring_buffer_peek() should return 20 which is at the head of the buffer after the previous advance");
    if (success == 0) success = assert_int_equality(10, *(int *) ring_buffer_peek(ring_buffer, 1), "ring_buffer_peek() with offset = 1 should return 10 which is at the end of the buffer after the previous advance");
    if (success != 0) return success;

    /* advance a third time to cause wrap-around */
    v3      = (int *) ring_buffer_advance(ring_buffer);
    *v3     = 30;
    success = assert_int_equality(30, *(int *) ring_buffer_peek(ring_buffer, 0), "ring_buffer_peek() should return 30 because we advanced and should have overwritten the 10");
    if (success == 0) success = assert_int_equality(20, *(int *) ring_buffer_peek(ring_buffer, 1), "ring_buffer_peek() with offset = 1 should return 20 which is at the end of the buffer");
    if (success == 0) success = assert_null(ring_buffer_peek(ring_buffer, 2), "ring_buffer_peek() with offset = 2 should return NULL because capacity is 2");
    if (success == 0) success = assert_int_equality(EINVAL, errno, "ring_buffer_peek() should set errno to EINVAL");

    ring_buffer_free(ring_buffer);
    return success;
}

int main(void) {
    const TestCase test_cases[] = {
        {"alloc", test_alloc},
        {"advanced and peek", test_advance_and_peek},
        {"wrap around", test_wrap_around}
    };
    const size_t test_count = sizeof(test_cases) / sizeof(test_cases[0]);
    return run_all_tests(test_cases, test_count);
}
