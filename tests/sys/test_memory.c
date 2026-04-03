//
// Created by martin on 28.03.2026.
//

#include "test.h"
#include "ctm/sys.h"

static int test_CtmMemory_Create(void) {
    // arrange
    // act
    CtmMemory* handle = CtmMemory_Create();

    // assert
    int success = assert_not_null(handle, "CtmMemory_Create() should not return NULL");
    if (success == 0) success = assert_unsigned_long_equality(0, CtmMemory_GetAvailable(handle), "CtmMemory_GetAvailable() should return 0");
    if (success == 0) success = assert_unsigned_long_equality(0, CtmMemory_GetFree(handle), "CtmMemory_GetFree() should return 0");
    if (success == 0) success = assert_unsigned_long_equality(0, CtmMemory_GetTotal(handle), "CtmMemory_GetTotal() should return 0");

    // cleanup and return
    CtmMemory_Destroy(handle);
    return success;
}

int main(void) {
    const TestCase test_cases[] = {
        {.name = "test_CtmMemory_Create", .func = test_CtmMemory_Create}
    };
    const size_t test_count = sizeof(test_cases) / sizeof(test_cases[0]);
    return run_all_tests(test_cases, test_count);
}
