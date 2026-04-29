#include "ui.h"
#include <ncurses.h>
#include <panel.h>
#include <stdlib.h>
#include "list.h"
#include "platform.h"
#include "process.h"

#define COLUMN_SPACER 2
#define ROW_SPACER 1
#define COLUMN_COUNT 3

#define HEADER_ROW_NUM 3
#define HEADER_SEPARATOR_ROW_NUM (HEADER_ROW_NUM + ROW_SPACER)
#define DATA_ROW_STARTING_NUM (HEADER_SEPARATOR_ROW_NUM + ROW_SPACER)

struct cell_t {
    char title[256];
    int  width;
    int  align;
};

static const struct cell_t g_header_cells[COLUMN_COUNT] = {
    {"PID", 6, 1},
    {"Name", 50, 1},
    {"State", 10, 1}
};

static list_node_t g_process_list;

static void init_ncurses(void)
{
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
}

static void draw_header(void)
{
    size_t i;
    int    offset = COLUMN_SPACER;

    attron(COLOR_PAIR(1));
    attron(A_BOLD | A_STANDOUT);
    mvprintw(1, 2, "[ Processes ]");
    attroff(A_BOLD | A_STANDOUT);
    attroff(COLOR_PAIR(1));

    for (i = 0; i < COLUMN_COUNT; i++) {
        mvprintw(HEADER_ROW_NUM, offset, "%-*s", g_header_cells[i].width, g_header_cells[i].title);
        mvhline(HEADER_SEPARATOR_ROW_NUM, offset, '-', g_header_cells[i].width);
        offset += g_header_cells[i].width + COLUMN_SPACER;
    }
}

static void draw_row(struct cell_t cells[],
                     const int     row_num)
{
    size_t i;
    int    offset = COLUMN_SPACER;

    for (i = 0; i < COLUMN_COUNT; i++) {
        mvprintw(row_num, offset, "%-*s", cells[i].width, cells[i].title);
        offset += cells[i].width + COLUMN_SPACER;
    }
}

static void draw_process_list(const void* ignored)
{
    list_node_t* curr;
    int          i = 0;

    (void) ignored;

    LIST_FOREACH(curr, &g_process_list) {
        process_t*    process;
        unsigned int  pid;
        char          name[128];
        int           is_active;
        struct cell_t cells[3];

        process = malloc(process_sizeof());
        process_from_node(curr, &process);
        process_pid(process, &pid);
        process_name(process, name, 100);
        process_active(process, &is_active);

        ctm_snprintf(cells[0].title, sizeof(cells[0].title), "%u", pid);
        cells[0].width = g_header_cells[0].width;
        cells[0].align = g_header_cells[0].align;

        ctm_snprintf(cells[1].title, sizeof(cells[1].title), "%s", name);
        cells[1].width = g_header_cells[1].width;
        cells[1].align = g_header_cells[1].align;

        ctm_snprintf(cells[2].title, sizeof(cells[2].title), "%s", is_active ? "Running" : "Stopped");
        cells[2].width = g_header_cells[2].width;
        cells[2].align = g_header_cells[2].align;

        draw_row(cells, DATA_ROW_STARTING_NUM + i++);
    }
}

int ui_start(int    argc,
             char** argv)
{
    (void) argc;
    (void) argv;

    init_ncurses();
    update_panels();
    draw_header();
    list_init(&g_process_list);
    process_list_refresh(&g_process_list,
                         128,
                         (process_callback_t) draw_process_list,
                         (process_callback_t) draw_process_list,
                         (process_callback_t) draw_process_list);
    doupdate();
    getch();
    endwin();

    return 0;
}
