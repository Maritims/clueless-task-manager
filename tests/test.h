#ifndef TEST_H
#define TEST_H
#include <stddef.h>

typedef int (*TestFunc)(void);

typedef struct TestCase {
    const char* name;
    TestFunc    func;
} TestCase;

int assert_int_equality(int expected, int actual, const char* message);
int assert_long_greater_than(long expected, long actual, const char* message);
int assert_unsigned_int_equality(unsigned int expected, unsigned int actual, const char* message);
int assert_unsigned_long_equality(unsigned long expected, unsigned long actual, const char* message);
int assert_null(const void* value, const char* message);
int assert_not_null(const void* value, const char* message);

int run_all_tests(const TestCase* test_cases, size_t test_count);

#endif
