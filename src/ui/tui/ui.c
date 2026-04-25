#include <ncurses.h>
#include <panel.h>

#include "ui/ui.h"

#include <stdlib.h>

#include "core/list.h"
#include "internal/ctm_process_tab_internal.h"
#include "internal/ctm_process_metrics_internal.h"
#include "ui/ctm_process_tab.h"

int ui_start(int argc, char** argv) {
    ProcessTab             process_tab;
    ctm_list_node_t        process_list;
    ctm_list_node_t*       curr;
    int                    i = 6;

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

    ctm_list_init(&process_list);
    process_manager_refresh(&process_list, 128, NULL, NULL, NULL);

    update_panels();

    attron(COLOR_PAIR(1));
    attron(A_BOLD | A_STANDOUT);
    mvprintw(1, 2, "[ Processes ]");
    attroff(A_BOLD | A_STANDOUT);
    attroff(COLOR_PAIR(1));

    mvprintw(3, 2, "PID     Name                  State");
    mvprintw(4, 2, "------  --------------------  -----");
    mvprintw(5, 2, "%6s  %-100s  %-6s", "123", "FooBar", "Running");

    ctm_list_for_each(curr, &process_list) {
        const ctm_process_metrics_t* process_metrics = ctm_list_entry(curr, ctm_process_metrics_t, node);
        unsigned int                 pid;
        char                         name[128];
        int                          is_active;

        ctm_process_metrics_get_pid(process_metrics, &pid);
        ctm_process_metrics_get_name(process_metrics, name, 100);
        ctm_process_metrics_get_is_active(process_metrics, &is_active);
        mvprintw(i, 2, "%u %s %s", pid, name, is_active ? "Running" : "Stopped");

        i++;
    }

    doupdate();

    getch();

    endwin();
    return 0;
}
