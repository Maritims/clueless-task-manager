#include <ncurses.h>
#include <panel.h>
#include <stdlib.h>

#include "ui.h"
#include "log/log.h"

struct ProcessTab {
    char*   label;
    WINDOW* window;
    PANEL*  panel;
};

ProcessTab* process_tab_create(void) {
    ProcessTab* process_tab = malloc(sizeof(ProcessTab));
    if (process_tab == NULL) {
        LOG_ERROR("process_tab_create", "process_tab cannot be NULL");
        return NULL;
    }

    process_tab->label  = "Processes";
    process_tab->window = newwin(LINES * 2, COLS * 2, 1, 0);
    process_tab->panel  = new_panel(process_tab->window);

    return process_tab;
}

void process_tab_destroy(ProcessTab* tab) {
    if (tab) {
        del_panel(tab->panel);
        delwin(tab->window);
        free(tab);
    }
}

size_t process_tab_update(ProcessTab* tab, Process* process_list) {
    if (tab == NULL || tab->window == NULL) {
        LOG_ERROR("process_tab_update", "tab cannot be NULL");
        return 0;
    }

    werase(tab->window);

    mvwprintw(tab->window, 0, 0, "PID   PROCESS");
    mvwprintw(tab->window, 1, 0, "------------------------------");

    Process* curr = process_list;
    size_t count = 0;
    while (curr) {
        LOG_INFO("process_tab_update", "Yay!");
        curr = process_get_next(curr);
        count++;
    }

    prefresh(tab->window, 0, 0, 0, 0, LINES - 1, COLS);
    return count;
}

void process_tab_register(ProcessTab* tab, void* tab_container) {
    (void) tab;
    (void) tab_container;
}