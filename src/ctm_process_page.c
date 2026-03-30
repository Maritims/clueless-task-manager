//
// Created by martin on 28.03.2026.
//

#include <gtk/gtk.h>

#include "ctm_process_page.h"

static int find_pid_in_process_store(const unsigned int target_pid, GtkTreeStore *process_store, GtkTreeIter *iter) {
    if (target_pid == 0) {
        fprintf(stderr, "find_pid_in_process_store: target_pid cannot be 0\n");
        return errno = EINVAL;
    }
    if (process_store == NULL) {
        fprintf(stderr, "find_pid_in_process_store: process_store cannot be NULL\n");
        return errno = EINVAL;
    }

    gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(process_store), iter);
    while (valid) {
        unsigned int current_pid;
        gtk_tree_model_get(GTK_TREE_MODEL(process_store), iter, CTM_PROCESS_PAGE_PID_COLUMN, &current_pid, -1);

        if (current_pid == target_pid) {
            return 0;
        }

        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(process_store), iter);
    }

    return -1;
}

static void ctm_toggle_show_processes_from_all_users(GtkToggleButton *toggle_button, gpointer data) {
    (void) toggle_button;
    if (data == NULL) {
        fprintf(stderr, "on_show_processes_from_all_users_toggled: data cannot be NULL\n");
        return;
    }

    CtmAppContext *ctx = data;
    ctm_app_context_set_show_processes_from_all_users(ctx, gtk_toggle_button_get_active(toggle_button));
    ctm_process_page_refresh(ctx);
}

void ctm_end_task(GtkButton *btn, gpointer data) {
    (void) btn;
    if (data == NULL) {
        fprintf(stderr, "on_end_task_btn_clicked: data cannot be NULL\n");
        return;
    }

    CtmAppContext *ctx          = data;
    GtkTreeView *  process_view = ctm_app_context_get_process_view(ctx);

    if (process_view == NULL) {
        fprintf(stderr, "on_end_task_btn_clicked: ctm_app_context_get_process_view returned NULL\n");
        return;
    }

    GtkTreeSelection *selection = gtk_tree_view_get_selection(process_view);
    GtkTreeModel *    model;
    GtkTreeIter       iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gint   pid;
        gchar *name;

        gtk_tree_model_get(model, &iter, CTM_PROCESS_PAGE_PID_COLUMN, &pid, CTM_PROCESS_PAGE_NAME_COLUMN, &name, -1);

        if (pid > 1) {
            if (kill(pid, SIGKILL) == 0) {
                g_print("Killed task %s\n", name);
            } else {
                fprintf(stderr, "Failed to kill task %s: %s\n", name, strerror(errno));
            }
        } else {
            fprintf(stderr, "Cannot end task of PID %d\n", pid);
        }

        g_free(name);
    } else {
        g_print("No process selected\n");
    }
}

GtkWidget *ctm_process_page_create(CtmAppContext *ctx) {
    if (ctx == NULL) {
        fprintf(stderr, "create_process_page: ctx cannot be NULL\n");
        return NULL;
    }

    // Tree view.
    GtkTreeStore *     store           = gtk_tree_store_new(CTM_PROCESS_PAGE_COLUMNS,G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN); // PID, Name, State, Updated.
    GtkWidget *        tree_view       = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    GtkCellRenderer *  renderer        = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *name_column     = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", CTM_PROCESS_PAGE_NAME_COLUMN, NULL);
    GtkTreeViewColumn *state_column    = gtk_tree_view_column_new_with_attributes("State", renderer, "text", CTM_PROCESS_PAGE_STATUS_COLUMN, NULL);
    GtkTreeViewColumn *username_column = gtk_tree_view_column_new_with_attributes("Username", renderer, "text", CTM_PROCESS_PAGE_USERNAME_COLUMN, NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), name_column);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), state_column);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), username_column);

    // Scrolled window.
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(tree_view));

    // Footer
    GtkWidget *footer_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(footer_box), 5);

    // Checkbox to show processes from all users.
    GtkWidget *show_all_users_checkbox = gtk_check_button_new_with_label("Show processes from all users");
    gtk_box_pack_start(GTK_BOX(footer_box), show_all_users_checkbox, FALSE, FALSE, 0);
    g_signal_connect(show_all_users_checkbox, "toggled", G_CALLBACK(ctm_toggle_show_processes_from_all_users), ctx);

    // Buttons.
    GtkWidget *btn_box      = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    GtkWidget *end_task_btn = gtk_button_new_with_label("End Task");
    g_signal_connect(end_task_btn, "clicked", G_CALLBACK(ctm_end_task), ctx);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(btn_box), GTK_BUTTONBOX_END);
    gtk_container_add(GTK_CONTAINER(btn_box), end_task_btn);
    gtk_box_pack_end(GTK_BOX(footer_box), btn_box, FALSE, FALSE, 0);

    // Layout.
    GtkWidget *layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(layout), scrolled_window, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(layout), footer_box, FALSE, FALSE, 5);

    // Persist in context.
    ctm_app_context_set_process_store(ctx, store);
    ctm_app_context_set_process_view(ctx, GTK_TREE_VIEW(tree_view));

    return layout;
}

int ctm_process_page_refresh(const CtmAppContext *ctx) {
    CtmProcessArray processes;
    GtkTreeIter     iter;

    if (ctm_process_array_init(&processes) != 0) {
        fprintf(stderr, "Failed to init process array: %s\n", strerror(errno));
        return -1;
    }
    if (ctm_process_fetch_all(&processes, ctm_app_context_get_show_processes_from_all_users(ctx)) != 0) {
        fprintf(stderr, "Failed to fetch process array: %s\n", strerror(errno));
        return -1;
    }

    GtkTreeStore *process_store = ctm_app_context_get_process_store(ctx);
    if (process_store == NULL) {
        fprintf(stderr, "ctm_process_page_refresh: ctm_app_context_get_process_store returned NULL\n");
        return -1;
    }

    // Mark all existing rows as removed.
    gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(process_store), &iter);
    while (valid) {
        gtk_tree_store_set(process_store, &iter, CTM_PROCESS_PAGE_UPDATED_COLUMN, FALSE, -1);
        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(process_store), &iter);
    }

    // Update all existing rows and create new rows.
    for (size_t i = 0; i < processes.count; i++) {
        GtkTreeIter target_iter;
        CtmProcess *process = &processes.elements[i];

        if (process->pid == 0) {
            fprintf(stderr, "ctm_process_page_refresh: process->pid cannot be 0\n");
            continue;
        }

        // Is the current process PID in the store?
        const int search_result = find_pid_in_process_store(process->pid, process_store, &target_iter);
        char      status_buffer[16];
        ctm_process_translate_state(process, status_buffer);

        if (search_result == 0) {
            // We found an existing row. Update it.
            gtk_tree_store_set(process_store, &target_iter,
                               CTM_PROCESS_PAGE_PID_COLUMN, process->pid,
                               CTM_PROCESS_PAGE_NAME_COLUMN, process->name,
                               CTM_PROCESS_PAGE_STATUS_COLUMN, status_buffer,
                               CTM_PROCESS_PAGE_USERNAME_COLUMN, process->username,
                               CTM_PROCESS_PAGE_UPDATED_COLUMN, TRUE,
                               -1);
        } else if (search_result == -1) {
            // This row does not exist. Create it.
            gtk_tree_store_append(process_store, &target_iter, NULL);
            gtk_tree_store_set(process_store, &target_iter,
                               CTM_PROCESS_PAGE_PID_COLUMN, process->pid,
                               CTM_PROCESS_PAGE_NAME_COLUMN, process->name,
                               CTM_PROCESS_PAGE_STATUS_COLUMN, status_buffer,
                               CTM_PROCESS_PAGE_USERNAME_COLUMN, process->username,
                               CTM_PROCESS_PAGE_UPDATED_COLUMN, TRUE,
                               -1);
        }
    }

    // Remove rows which weren't updated or created.
    valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(process_store), &iter);
    while (valid) {
        gboolean updated;
        gtk_tree_model_get(GTK_TREE_MODEL(process_store), &iter,
                           CTM_PROCESS_PAGE_UPDATED_COLUMN, &updated,
                           -1);
        valid = updated ? gtk_tree_model_iter_next(GTK_TREE_MODEL(process_store), &iter) : gtk_tree_store_remove(process_store, &iter);
    }

    return 0;
}
