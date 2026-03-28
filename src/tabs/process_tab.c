//
// Created by martin on 28.03.2026.
//

#include <gtk/gtk.h>

#include "tabs/process_tab.h"
#include "ui.h"

static gboolean find_pid_in_store(GtkTreeStore *store, const int pid, GtkTreeIter *iter) {
    gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), iter);

    while (valid) {
        int current_pid;
        gtk_tree_model_get(GTK_TREE_MODEL(store), iter, PROCESS_TAB_PID_COLUMN, &current_pid, -1);

        if (current_pid == pid) {
            return TRUE;
        }

        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(store), iter);
    }

    return FALSE;
}

static void state_to_status(const process_t *process, char *status) {
    switch (process->state) {
        case 'R':
            strncpy(status, "Running", 7);
            status[7] = '\0';
            break;
        case 'S':
            strncpy(status, "Sleeping", 8);
            status[8] = '\0';
            break;
        case 'D':
            strncpy(status, "Disk Sleep", 10);
            status[10] = '\0';
            break;
        case 'Z':
            strncpy(status, "Zombie", 7);
            status[7] = '\0';
            break;
        case 'T':
            strncpy(status, "Traced/Stopped", 14);
            status[14] = '\0';
            break;
        case 't':
            strncpy(status, "Stopped", 7);
            status[7] = '\0';
            break;
        case 'W':
            strncpy(status, "Paging", 7);
            status[7] = '\0';
            break;
        case 'X':
        case 'x':
            strncpy(status, "Dead", 4);
            status[4] = '\0';
            break;
        case 'K':
            strncpy(status, "Wake-kill", 7);
            status[7] = '\0';
            break;
        case 'P':
            strncpy(status, "Parked", 6);
            status[6] = '\0';
            break;
        case 'I':
            strncpy(status, "Idle", 4);
            status[4] = '\0';
            break;
        default:
            fprintf(stderr, "Unknown state: %c\n", process->state);
            strncpy(status, "Unknown", 7);
            status[7] = '\0';
            break;
    }
}

GtkWidget *process_tab_create(app_context_t *ctx) {
    // Initialise the store directly into the context
    ctx->process_store = gtk_tree_store_new(PROCESS_TAB_NUM_COLUMNS,
                                            G_TYPE_INT,     // PID
                                            G_TYPE_BOOLEAN, // Updated
                                            G_TYPE_STRING,  // Task
                                            G_TYPE_STRING); // Status

    // Create the tree view with the store we just made
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(ctx->process_store));

    // Set up the renderer and the columns
    GtkCellRenderer *  renderer      = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *task_column   = gtk_tree_view_column_new_with_attributes("Task", renderer, "text", PROCESS_TAB_TASK_NAME_COLUMN, NULL);
    GtkTreeViewColumn *status_column = gtk_tree_view_column_new_with_attributes("Status", renderer, "text", PROCESS_TAB_TASK_STATUS_COLUMN, NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), task_column);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), status_column);

    // Create the scrolled window
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);

    // Create the tab container
    GtkWidget *notebook_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(notebook_page), 10);

    // Pack the scrolled window (which now contains the list)
    gtk_box_pack_start(GTK_BOX(notebook_page), scrolled_window, TRUE, TRUE, 0);

    // Create button box
    GtkWidget *button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_END);
    gtk_box_set_spacing(GTK_BOX(button_box), 10);

    // Add buttons
    GtkWidget *end_btn = gtk_button_new_with_label("End Task");
    gtk_container_add(GTK_CONTAINER(button_box), end_btn);

    // Pack the button box
    gtk_box_pack_start(GTK_BOX(notebook_page), button_box, FALSE, FALSE, 0);

    return notebook_page;
}

void process_tab_update(GtkTreeStore *store, process_array_t *processes) {
    if (!store || !processes) {
        fprintf(stderr, "Invalid argument: NULL\n");
        return;
    }

    GtkTreeIter iter;

    // Mark everything as "not updated"
    gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);
    while (valid) {
        gtk_tree_store_set(store, &iter, PROCESS_TAB_UPDATED_COLUMN, FALSE, -1);
        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter);
    }

    // Add or update
    for (size_t i = 0; i < processes->count; i++) {
        process_t *process = &processes->elements[i];
        char status[16];
        state_to_status(process, status);

        if (find_pid_in_store(store, process->pid, &iter)) {
            gtk_tree_store_set(store, &iter,
                               PROCESS_TAB_TASK_STATUS_COLUMN, status,
                               PROCESS_TAB_UPDATED_COLUMN, TRUE,
                               -1);
        } else {

            gtk_tree_store_append(store, &iter, NULL);
            gtk_tree_store_set(store, &iter,
                               PROCESS_TAB_PID_COLUMN, process->pid,
                               PROCESS_TAB_TASK_NAME_COLUMN, process->name,
                               PROCESS_TAB_TASK_STATUS_COLUMN, status,
                               PROCESS_TAB_UPDATED_COLUMN, TRUE,
                               -1
            );
        }
    }

    // Remove dead processes
    valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);
    while (valid) {
        gboolean updated;
        gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, PROCESS_TAB_UPDATED_COLUMN, &updated, -1);

        valid = updated ? gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter) : gtk_tree_store_remove(store, &iter);
    }
}
