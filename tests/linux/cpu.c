#include <stdlib.h>

#include "../test.h"
#include "ctm/ctm.h"

int test_get(void) {
    /* arrange */
    int  success;
    CPU* actual;

    /* act */
    actual = cpu_get();
    cpu_free(actual);

    /* assert */
    success = assert_not_null(actual, "cpu_get() should not return NULL");

    return success;
}

int main(void) {
    const TestCase test_cases[] = {
        {"cpu_get", test_get},
    };
    const size_t test_count = sizeof(test_cases) / sizeof(test_cases[0]);
    return run_all_tests(test_cases, test_count);
}
