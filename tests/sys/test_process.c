//
// Created by martin on 27.03.2026.
//

#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "ctm/sys.h"

static int CIO_READ_FILE_INVOCATIONS;

char* cio_read_file(const char* path, size_t* out_size) {
    CIO_READ_FILE_INVOCATIONS++;
    char* buffer = malloc(1024);
    strcpy(buffer, "cpu  9236370 6916 2562494 359278916 220954 0 720624 0 0 0");
    return buffer;
}

static int test_CtmProcessArray_Create(void) {
    // arrange
    // act
    CtmProcessArray* array = CtmProcessArray_Create();

    // assert
    int success = assert_not_null(array, "CtmProcessArray_Create() should not return NULL");
    if (success == 0) success = assert_unsigned_long_equality(0, CtmProcessArray_GetCount(array), "CtmProcessArray_GetCount() should return 0");
    if (success == 0) success = assert_unsigned_long_equality(0, CtmProcessArray_GetCapacity(array), "CtmProcessArray_GetCapacity() should return 0");
    if (success == 0) success = assert_null(CtmProcessArray_GetElements(array), "CtmProcessArray_GetElements() should return NULL");

    // cleanup and return
    CtmProcessArray_Destroy(array);
    return success;
}

static int test_CtmProcessArray_Destroy(void) {
    // arrange
    CtmProcessArray*  array    = CtmProcessArray_Create();
    const CtmProcess* elements = CtmProcessArray_GetElements(array);

    // act
    CtmProcessArray_Destroy(array);

    // assert
    const int success = assert_null(elements, "CtmProcessArray_Destroy() should set elements to NULL");

    // clean-up and return
    return success;
}

static int test_CtmProcess_GetUsageScaled(void) {
    // arrange
    CtmProcess* process = CtmProcess_Create(0, 0, "Foo Bar", "Sleeping", "Foo Bar", 4160, 1260, 0, 0);

    // act
    const unsigned long long usage_scaled = CtmProcess_GetCPUUsageScaled(process);

    // assert
    const int success = assert_unsigned_long_equality(0, usage_scaled, "CtmProcess_GetCPUUsageScaled() should return 0");

    // clean-up and return
    CtmProcess_Destroy(process);
    return success;
}

int main(void) {
    const TestCase test_cases[] = {
        {"CtmProcessArray_Create", test_CtmProcessArray_Create},
        {"CtmProcessArray_Destroy", test_CtmProcessArray_Destroy},
        {"CtmProcess_GetUsageScaled", test_CtmProcess_GetUsageScaled}
    };
    const size_t test_count = sizeof(test_cases) / sizeof(test_cases[0]);
    return run_all_tests(test_cases, test_count);
}
