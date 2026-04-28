#include <stdio.h>
#include "list.h"
#include "process.h"
#include "process_internal.h"
#include "test.h"

TEST(process_manager_refresh)
{
    /* arrange */
    list_node_t process_list;
    int         success;

    list_init(&process_list);

    /* act */
    success = process_list_refresh(&process_list, 1, NULL, NULL, NULL);

    /* assert */
    assert_int_equality(0, success, "process_manager_refresh() should return 0");

    /* cleanup */
    {
        list_node_t* curr;
        list_node_t* n;
        LIST_FOREACH_SAFE(curr, n, &process_list) {
            process_t* process_metrics = LIST_ENTRY(curr, process_t, node);
            list_delete_node(curr);
            process_destroy(process_metrics);
        }
    }

    return success;
}

#define TESTS \
    TEST_CASE(process_manager_refresh)

RUN_TEST_SUITE(TESTS)
