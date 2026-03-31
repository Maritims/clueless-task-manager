//
// Created by martin on 27.03.2026.
//

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ctm_process.h"

void test_ctm_process_array_new(void) {
    CtmProcessArray* array = ctm_process_array_new();
    assert(array != NULL);
    assert(ctm_process_array_get_elements(array) == NULL);
    assert(ctm_process_array_get_count(array) == 0);
    assert(ctm_process_array_get_capacity(array) == 0);
    ctm_process_array_free(array);
    printf("%s passed\n", __func__);
}

void test_ctm_process_array_free(void) {
    CtmProcessArray*  array    = ctm_process_array_new();
    const CtmProcess* elements = ctm_process_array_get_elements(array);
    ctm_process_array_free(array);
    assert(elements == NULL);
    printf("%s passed\n", __func__);
}

int main(void) {
    test_ctm_process_array_new();
    test_ctm_process_array_free();
    printf("All tests passed\n");
}
