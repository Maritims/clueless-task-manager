//
// Created by martin on 28.03.2026.
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

static int test_CtmSystemInfo_Create(void) {
    // arrange
    // act
    CtmSystemInfo* handle = CtmSystemInfo_Create();

    // assert
    int success = assert_not_null(handle, "CtmSystemInfo_Create() should not return NULL");
    if (success == 0) success = assert_null(CtmSystemInfo_GetCpu(handle), "CtmSystemInfo_GetCpu() should return NULL");
    if (success == 0) success = assert_null(CtmSystemInfo_GetMemory(handle), "CtmSystemInfo_GetMemory() should return NULL");
    if (success == 0) success = assert_null(CtmSystemInfo_GetProcesses(handle), "CtmSystemInfo_GetProcesses() should return NULL");

    // clean-up adn return
    CtmSystemInfo_Destroy(handle);
    return success;
}

int main(void) {
    TestCase test_cases[] = {
        {"CtmSystemInfo_Create", test_CtmSystemInfo_Create}
    };
    const size_t test_count = sizeof(test_cases) / sizeof(test_cases[0]);
    return run_all_tests(test_cases, test_count);
}
