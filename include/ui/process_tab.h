#ifndef CTM_PROCESS_TAB_H
#define CTM_PROCESS_TAB_H
#include <stddef.h>

#include "list.h"

typedef struct ProcessTab ProcessTab;

int process_tab_create(ProcessTab* out);

int process_tab_destroy(ProcessTab* tab);

int process_tab_update(ProcessTab*  tab,
                       list_node_t* process_list,
                       size_t*      out);

int process_tab_register(ProcessTab* tab,
                         void*       tab_container);

#endif
