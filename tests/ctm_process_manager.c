#include "metrics/ctm_process_manager.h"
#include <stdio.h>
#include "test.h"
#include "internal/ctm_process_metrics_internal.h"

TEST(process_manager_refresh) {
    /* arrange */
    ctm_process_metrics_t process_list;
    int                   success;

    /* act */
    success = process_manager_refresh(&process_list, 1, NULL, NULL, NULL);

    /* assert */
    assert_int_equality(0, success, "process_manager_refresh() should return 0");

    return success;
}

#define TESTS \
    TEST_CASE(process_manager_refresh)

RUN_TEST_SUITE(TESTS)
