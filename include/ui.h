//
// Created by martin on 26.03.2026.
//

#ifndef CLUELESS_TASK_MGR_UI_H
#define CLUELESS_TASK_MGR_UI_H

#define HISTORY_SIZE 100
#include <gtk/gtk.h>

#include "sysinfo.h"
#include "../modules/sysinfo/include/cpu.h"

typedef struct {
    // View
    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *cpu_bar;
    GtkWidget *memory_bar;
    GtkWidget *drawing_area;

    GtkTreeStore *process_store;

    // Model
    sysinfo_t   sysinfo;
    cpu_stats_t last_cpu_stats;
    double      cpu_history[HISTORY_SIZE];
    int         history_count;
} app_context_t;

/**
 * Creates and assembles the main application window.
 * @param ctx Pointer to the application context to be populated with widgets.
 * @return The top-level GtkWidget (the window).
 */
void ui_create_window(app_context_t *ctx);

#endif //CLUELESS_TASK_MGR_UI_H
