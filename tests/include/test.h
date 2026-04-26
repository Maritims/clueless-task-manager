#ifndef TEST_H
#define TEST_H
#include <stddef.h>

typedef int (*TestFunc)(void);

typedef struct TestCase {
    const char* name;
    TestFunc    func;
} TestCase;

#define TEST(name) \
    int test_##name(void)

#define TEST_CASE(name) {#name, test_##name },

#define RUN_TEST_SUITE(test_entries) \
    int main(void) { \
        const TestCase test_cases[] = { \
            test_entries \
        }; \
        return run_all_tests(test_cases, sizeof(test_cases) / sizeof(test_cases[0])); \
    }

int assert_int_equality(int         expected,
                        int         actual,
                        const char* message);

int assert_long_greater_than(long        expected,
                             long        actual,
                             const char* message);

int assert_unsigned_int_equality(unsigned int expected,
                                 unsigned int actual,
                                 const char*  message);

int assert_unsigned_long_equality(unsigned long expected,
                                  unsigned long actual,
                                  const char*   message);

int assert_null(const void* value,
                const char* message);

int assert_not_null(const void* value,
                    const char* message);

int assert_string_equality(const char* expected,
                           const char* actual,
                           const char* message);

int run_all_tests(const TestCase* test_cases,
                  size_t          test_count);

#endif
