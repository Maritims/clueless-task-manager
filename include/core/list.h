#ifndef CTM_CORE_LIST_H
#define CTM_CORE_LIST_H

#include <stddef.h>

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define ctm_list_entry(ptr, type, member) \
((type *)((char *)(ptr) - offsetof(type, member)))

/* A helper for iterating safely */
#define ctm_list_for_each(pos, head) \
for (pos = (head)->next; pos != (head); pos = pos->next)

typedef struct list_node {
    struct list_node *next;
    struct list_node *prev;
} ctm_list_node_t;

void ctm_list_add(ctm_list_node_t* head, ctm_list_node_t* node);

#endif