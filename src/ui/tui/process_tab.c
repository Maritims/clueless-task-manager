#include <ncurses.h>
#include <panel.h>
#include <stdlib.h>

#include "process_tab.h"
#include "list.h"
#include "internal/process_tab_internal.h"

int process_tab_create(ProcessTab* out_value)
{
    if (out_value == NULL) {
        return -1;
    }

    out_value->label  = "Processes";
    out_value->window = newwin(LINES * 2, COLS * 2, 1, 0);
    out_value->panel  = new_panel(out_value->window);

    return 0;
}

int process_tab_destroy(ProcessTab* tab)
{
    if (tab == NULL) {
        return -1;
    }

    del_panel(tab->panel);
    delwin(tab->window);
    free(tab);

    return 0;
}

int process_tab_update(ProcessTab*  tab,
                       list_node_t* process_list,
                       size_t*      out)
{
    list_node_t* curr;

    if (tab == NULL || tab->window == NULL || process_list == NULL) {
        return -1;
    }

    werase(tab->window);

    mvwprintw(tab->window, 0, 0, "PID   PROCESS");
    mvwprintw(tab->window, 1, 0, "------------------------------");

    LIST_FOREACH(curr, process_list) {
        (*out)++;
    }

    prefresh(tab->window, 0, 0, 0, 0, LINES - 1, COLS);
    return 0;
}

int process_tab_register(ProcessTab* tab,
                         void*       tab_container)
{
    (void) tab;
    (void) tab_container;

    return 0;
}
