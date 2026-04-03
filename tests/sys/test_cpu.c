//
// Created by martin on 28.03.2026.
//

#include <stdlib.h>
#include <string.h>

#include "../test.h"
#include "ctm/sys.h"

static int CIO_READ_FILE_INVOCATIONS;

char* cio_read_file(const char* path, size_t* out_size) {
    CIO_READ_FILE_INVOCATIONS++;
    char* buffer = malloc(1024);
    strcpy(buffer, "cpu  9236370 6916 2562494 359278916 220954 0 720624 0 0 0");
    return buffer;
}

static int test_CtmCpu_LoadFromKernel(void) {
    // arrange
    CIO_READ_FILE_INVOCATIONS = 0;

    // act
    CtmCpu* cpu = CtmCpu_LoadFromKernel();

    // assert
    int success = assert_int_equality(1, CIO_READ_FILE_INVOCATIONS, "cio_read_file() should have been invoked exactly once");
    if (success == 0) {
        success = assert_not_null(cpu, "cpu should not be NULL");
    }

    // cleanup
    CtmCpu_Destroy(cpu);

    return success;
}

static int test_CtmCpu_GetUsageScaled(void) {
    // Arrange
    CIO_READ_FILE_INVOCATIONS = 0;
    CtmCpu* cpu               = CtmCpu_LoadFromKernel();

    // Act
    const unsigned int actual = CtmCpu_GetUsageScaled(cpu);

    // Assert
    int success = assert_int_equality(1, CIO_READ_FILE_INVOCATIONS, "CtmCpu_LoadFromKernel() should have invoked cio_read_file() exactly once");
    if (success == 0) {
        success = assert_unsigned_int_equality(343, actual, "total_time should be 343");
    }

    // Cleanup and return
    CtmCpu_Destroy(cpu);
    return success;
}

int main(void) {
    const TestCase test_cases[] = {
        {.name = "CtmCpu_LoadFromKernel", .func = test_CtmCpu_LoadFromKernel},
        {.name = "CtmCpu_GetUsageScaled", .func = test_CtmCpu_GetUsageScaled}
    };
    const size_t test_count = sizeof(test_cases) / sizeof(test_cases[0]);
    return run_all_tests(test_cases, test_count);
}
