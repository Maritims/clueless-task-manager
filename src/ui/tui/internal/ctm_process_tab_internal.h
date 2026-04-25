#ifndef CTM_PROCESS_TAB_INTERNAL_H
#define CTM_PROCESS_TAB_INTERNAL_H

#include <ncurses.h>
#include <panel.h>

struct ProcessTab {
    char*   label;
    WINDOW* window;
    PANEL*  panel;
};

#endif
