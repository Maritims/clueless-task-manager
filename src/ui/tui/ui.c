#include <ncurses.h>
#include <panel.h>

#include "ui.h"

#include <stdlib.h>

#include "list.h"
#include "internal/process_tab_internal.h"
#include "process.h"
#include "process_tab.h"

int ui_start(int    argc,
             char** argv)
{
    ProcessTab   process_tab;
    list_node_t  process_list;
    list_node_t* curr;
    int          i = 6;

    (void) argc;
    (void) argv;

    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    process_tab_create(&process_tab);
    (void) process_tab;

    list_init(&process_list);
    process_list_refresh(&process_list, 128, NULL, NULL, NULL);

    update_panels();

    attron(COLOR_PAIR(1));
    attron(A_BOLD | A_STANDOUT);
    mvprintw(1, 2, "[ Processes ]");
    attroff(A_BOLD | A_STANDOUT);
    attroff(COLOR_PAIR(1));

    mvprintw(3, 2, "PID     Name                  State");
    mvprintw(4, 2, "------  --------------------  -----");
    mvprintw(5, 2, "%6s  %-100s  %-6s", "123", "FooBar", "Running");

    LIST_FOREACH(curr, &process_list) {
        process_t*   process;
        size_t       process_size;
        unsigned int pid;
        char         name[128];
        int          is_active;

        process_sizeof(&process_size);
        process = malloc(process_size);
        process_from_node(curr, &process);
        
        process_pid(process, &pid);
        process_name(process, name, 100);
        process_active(process, &is_active);
        mvprintw(i, 2, "%u %s %s", pid, name, is_active ? "Running" : "Stopped");

        i++;
    }

    doupdate();

    getch();

    endwin();
    return 0;
}
