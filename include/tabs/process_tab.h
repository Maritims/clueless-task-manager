//
// Created by martin on 28.03.2026.
//

#ifndef CLUELESS_TASK_MGR_PROCESS_TAB_H
#define CLUELESS_TASK_MGR_PROCESS_TAB_H
#include <gtk/gtk.h>

#include "ui.h"

typedef enum {
    PROCESS_TAB_PID_COLUMN,
    PROCESS_TAB_UPDATED_COLUMN,
    PROCESS_TAB_TASK_NAME_COLUMN,
    PROCESS_TAB_TASK_STATUS_COLUMN,
    PROCESS_TAB_NUM_COLUMNS
} process_column_t;

GtkWidget *process_tab_create(app_context_t *ctx);

void process_tab_update(GtkTreeStore *store, process_array_t *processes);

#endif //CLUELESS_TASK_MGR_PROCESS_TAB_H
