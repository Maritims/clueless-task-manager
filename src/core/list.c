#include "core/list.h"

void ctm_list_add(ctm_list_node_t* head, ctm_list_node_t* new_node) {
    /* Set the new node's neighbours */
    new_node->next = head->next;
    new_node->prev = head;

    head->next->prev = new_node;
    head->next       = new_node;
}
