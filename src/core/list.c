#include "list.h"

void list_init(list_node_t* head)
{
    head->next = head;
    head->prev = head;
}

void list_add_node(list_node_t* head,
                   list_node_t* new_node)
{
    /*
     * Consider the nodes A and B.
     * A: the new head.
     * B: the current head.
     *
     * We're pushing everything back to make room for A, take the following steps:
     * 1) Set the next node of A to the next node of B (to maintain circularity).
     * 2) Set the previous node of A to B.
     * 3) Set the previous node (prev) of the last element in the list (head->next) to A.
     * 4) Set the next node of B to A.
     */

    new_node->next   = head->next;
    new_node->prev   = head;
    head->next->prev = new_node;
    head->next       = new_node;
}

void list_delete_node(list_node_t* node)
{
    /*
     * Consider the nodes A, B and C.
     * A: the previous node
     * B: the target node
     * C: the next node
     *
     * We're removing B, take the following steps:
     * 1) Set the previous node of C to A, skipping B.
     * 2) Set the next node of A to C, skipping B.
     * 3) Clear B's pointers to prevent dangling references.
     */

    node->next->prev = node->prev;
    node->prev->next = node->next;
    node->next       = NULL;
    node->prev       = NULL;
}
