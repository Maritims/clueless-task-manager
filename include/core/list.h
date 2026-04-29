#ifndef CTM_CORE_LIST_H
#define CTM_CORE_LIST_H

#include <stddef.h>

#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define LIST_ENTRY(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/* A helper for iterating */
#define LIST_FOREACH(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/* A helper for iterating safely against removal of list entry */
#define LIST_FOREACH_SAFE(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

typedef struct list_node {
    struct list_node* next;
    struct list_node* prev;
    size_t*           count;
} list_node_t;

void list_init(list_node_t* head);

size_t list_count(const list_node_t* node);

void list_add_node(list_node_t* head,
                   list_node_t* node);

void list_delete_node(list_node_t* node);

#endif
