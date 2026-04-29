#include "ui.h"
#include <ncurses.h>
#include <stdlib.h>

#define COLUMN_SPACER 1
#define COLUMN_COUNT 3
#define INITIAL_PROCESS_LIST_COUNT 128
#define INITIAL_DATA_ROW_NUM 2

struct cell_t {
    char title[256];
    int  width;
    int  align;
};

static WINDOW* g_list_pad      = NULL;
static int     g_scroll_offset = 0;
static int     g_data_row_num  = INITIAL_DATA_ROW_NUM;

static const struct cell_t g_header_cells[COLUMN_COUNT] = {
    {"PID", 6, 1},
    {"Name", 50, 1},
    {"State", 10, 1}
};

static list_node_t g_process_list;

static void render_header(void)
{
    int i;
    int offset = COLUMN_SPACER;

    for (i = 0; i < COLUMN_COUNT; i++) {
        mvwprintw(g_list_pad, 0, offset, "%-*s", g_header_cells[i].width, g_header_cells[i].title);
        mvwhline(g_list_pad, 1, offset, '-', g_header_cells[i].width);
        offset += g_header_cells[i].width + COLUMN_SPACER;
    }

    clrtoeol();
}

static void render_process(const process_t* process)
{
    int          offset = COLUMN_SPACER;
    unsigned int pid;
    char         name[256];
    char         state[32];

    process_pid(process, &pid);
    process_name(process, name, 256);
    process_state(process, state, 32);

    mvwprintw(g_list_pad, g_data_row_num, offset, "%-*d", 6, pid);
    offset += g_header_cells[0].width + COLUMN_SPACER;
    mvwprintw(g_list_pad, g_data_row_num, offset, "%-*s", 50, name);
    offset += g_header_cells[1].width + COLUMN_SPACER;
    mvwprintw(g_list_pad, g_data_row_num, offset, "%-*s", 10, state);

    g_data_row_num++;
}

static void render_process_list(void)
{
    list_node_t* curr = NULL;
    g_data_row_num    = INITIAL_DATA_ROW_NUM;

    render_header();
    process_list_refresh(&g_process_list,
                         INITIAL_PROCESS_LIST_COUNT,
                         (process_callback_t) render_process,
                         (process_callback_t) render_process,
                         (process_callback_t) render_process);

    LIST_FOREACH(curr, &g_process_list) {
        process_t* process;
        process_from_node(curr, &process);
        render_process(process);
    }

    prefresh(g_list_pad, g_scroll_offset, 0, 1, 0, LINES - 1, COLS - 1);
}

int ui_start(int    argc,
             char** argv)
{
    int ch;

    (void) argc;
    (void) argv;

    list_init(&g_process_list);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    g_list_pad = newpad(100, COLS);
    render_header();
    refresh();
    prefresh(g_list_pad, g_scroll_offset, 0, 1, 0, LINES - 1, COLS - 1);

    while (1) {
        render_process_list();

        ch = getch();
        if (ch == 'q') {
            break;
        }

        switch (ch) {
            case KEY_UP:
                if (g_scroll_offset > 0) {
                    g_scroll_offset--;
                }
                break;
            case KEY_DOWN:
                if (g_scroll_offset < 100 - (LINES - 1)) {
                    g_scroll_offset++;
                }
                break;
            default: ;
        }
    }

    doupdate();
    delwin(g_list_pad);
    endwin();

    return 0;
}
