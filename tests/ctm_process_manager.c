#include "metrics/process_manager.h"
#include <stdio.h>
#include "test.h"
#include "../include/ctm_process_metrics.h"

TEST(process_manager_refresh) {
    /* arrange */
    Process*      process_list = NULL;
    Process*      curr;
    int           success;

    /* act */
    success = process_manager_refresh(&process_list, NULL, NULL, NULL);

    /* assert */
    assert_int_equality(0, success, "process_manager_refresh() should return 0");

    curr = process_list;
    while (curr) {
        unsigned int pid = process_get_pid(curr);
        const char* name = process_get_name(curr);
        printf("pid: %u - name: %s\n", pid, name);
        curr = process_get_next(curr);
    }

    /* cleanup */
    while (process_list) {
        curr = process_list;
        process_list = process_get_next(process_list);
        process_destroy(curr);
    }

    return success;
}

#define TESTS \
    TEST_CASE(process_manager_refresh)

RUN_TEST_SUITE(TESTS)
