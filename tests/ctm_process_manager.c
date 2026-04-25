#include "metrics/ctm_process_manager.h"
#include <stdio.h>
#include <stdlib.h>

#include "test.h"
#include "internal/ctm_process_metrics_internal.h"

TEST(process_manager_refresh) {
    /* arrange */
    ctm_list_node_t process_list;
    int             success;

    ctm_list_init(&process_list);

    /* act */
    success = process_manager_refresh(&process_list, 1, NULL, NULL, NULL);

    /* assert */
    assert_int_equality(0, success, "process_manager_refresh() should return 0");

    /* cleanup */
    {
        ctm_list_node_t* curr;
        ctm_list_node_t* n;
        ctm_list_for_each_safe(curr, n, &process_list) {
            ctm_process_metrics_t* process_metrics = ctm_list_entry(curr, ctm_process_metrics_t, node);
            ctm_list_del(curr);
            ctm_process_metrics_destroy(process_metrics);
        }
    }

    return success;
}

#define TESTS \
    TEST_CASE(process_manager_refresh)

RUN_TEST_SUITE(TESTS)
