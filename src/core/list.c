#include "core/list.h"

void ctm_list_add(ctm_list_node_t* head, ctm_list_node_t* new_node) {
    /* Set the new node's neighbours */
    new_node->next = head->next;
    new_node->prev = head;

    head->next->prev = new_node;
    head->next       = new_node;
}

void ctm_list_del(ctm_list_node_t* node) {
    node->next->prev = node->prev;
    node->prev->next = node->next;
    node->next       = NULL;
    node->prev       = NULL;
}

void ctm_list_init(ctm_list_node_t* head) {
    head->next = head;
    head->prev = head;
}
