#include "ui.h"

#include <ncurses.h>
#include <panel.h>

#include "log/log.h"

int ui_start(int argc, char** argv) {
    (void) argc;
    (void) argv;

    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    ProcessTab* process_tab = process_tab_create();
    (void) process_tab;

    Process*     process_list = NULL;
    process_manager_refresh(process_list, NULL, NULL, NULL);

    update_panels();

    attron(COLOR_PAIR(1));
    attron(A_BOLD | A_STANDOUT);
    mvprintw(1, 2, "[ Processes ]");
    attroff(A_BOLD | A_STANDOUT);
    attroff(COLOR_PAIR(1));

    mvprintw(3, 2, "PID     Name                  State");
    mvprintw(4, 2, "------  --------------------  -----");
    mvprintw(5, 2, "%6s  %-100s  %-6s", "123", "FooBar", "Running");

    Process*      curr = process_list;

    int i = 6;
    while (curr) {
        unsigned int pid = process_get_pid(curr);
        const char* name = process_get_name(curr);
        mvprintw(i, 2, "%u %s %s", pid, name, process_get_is_active(curr) ? "Running" : "Stopped");
        i++;
        curr = process_get_next(curr);
    }

    doupdate();

    getch();

    endwin();
    return 0;
}
