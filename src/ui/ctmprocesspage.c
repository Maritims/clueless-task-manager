//
// Created by martin on 28.03.2026.
//

#include <assert.h>
#include <gtk/gtk.h>
#include <stdbool.h>

#include "ctm.h"
#include "ctm_cpu.h"
#include "ctm_sound.h"

typedef struct {
    const char* title;
    int         column_id;
    GType       type;
    gboolean    visible;
} CtmProcessPageColumn;

struct CtmProcessPage {
    GtkWidget*    widget;
    GtkTreeStore* process_store;
    GtkWidget*    process_view;
    bool          include_processes_from_all_users;
};

static const CtmProcessPageColumn PROCESS_PAGE_COLUMNS[] = {
    {"PID", CTM_PROCESS_PAGE_PID_COLUMN, G_TYPE_INT, FALSE},
    {"Image Name", CTM_PROCESS_PAGE_NAME_COLUMN, G_TYPE_STRING, TRUE},
    {"User Name", CTM_PROCESS_PAGE_USERNAME_COLUMN, G_TYPE_STRING, TRUE},
    {"CPU", CTM_PROCESS_PAGE_CPU_COLUMN, G_TYPE_STRING, TRUE},
    {"State", CTM_PROCESS_PAGE_STATUS_COLUMN, G_TYPE_STRING, TRUE},
    {"Total Time", CTM_PROCESS_PAGE_TIME_COLUMN, G_TYPE_STRING, TRUE},
    {"Updated", CTM_PROCESS_PAGE_UPDATED_COLUMN, G_TYPE_BOOLEAN, FALSE}
};

static int find_pid_in_process_store(const unsigned int target_pid, GtkTreeStore* process_store, GtkTreeIter* iter) {
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

static void toggle_show_processes_from_all_users(GtkToggleButton* toggle_button, gpointer self) {
    (void) toggle_button;
    if (self == NULL) {
        fprintf(stderr, "%s: data cannot be NULL\n", __func__);
        return;
    }

    CtmProcessPage* process_page = self;
    ctm_process_page_set_include_processes_from_all_users(process_page, gtk_toggle_button_get_active(toggle_button));
}

static void set_row_from_process(GtkTreeStore* store, GtkTreeIter* iter, const CtmProcess* process) {
    if (process == NULL) {
        fprintf(stderr, "set_row_from_process: process cannot be NULL\n");
        return;
    }

    char duration_buffer[32];
    ctm_format_duration(ctm_process_get_total_time(process), duration_buffer, sizeof(duration_buffer));

    CtmCpuStats*       current   = ctm_cpu_stats_new();
    const unsigned int cpu_usage = ctm_cpu_stats_get_usage_scaled(current);
    char               cpu_usage_buffer[32];
    snprintf(cpu_usage_buffer, sizeof(cpu_usage_buffer), "%d", cpu_usage);
    ctm_cpu_stats_free(current);


    gtk_tree_store_set(store, iter,
                       CTM_PROCESS_PAGE_PID_COLUMN, ctm_process_get_pid(process),
                       CTM_PROCESS_PAGE_NAME_COLUMN, ctm_process_get_name(process),
                       CTM_PROCESS_PAGE_STATUS_COLUMN, ctm_process_get_state(process),
                       CTM_PROCESS_PAGE_USERNAME_COLUMN, ctm_process_get_username(process),
                       CTM_PROCESS_PAGE_CPU_COLUMN, cpu_usage_buffer,
                       CTM_PROCESS_PAGE_TIME_COLUMN, duration_buffer,
                       CTM_PROCESS_PAGE_UPDATED_COLUMN, TRUE,
                       -1);
}

static gboolean on_sound_finished_idle(gpointer data) {
    ctm_sound_stop(data);
    return FALSE;
}

static void sound_finish_bridge(CtmAudioStreamSource* audio_stream_source, void* userdata) {
    g_idle_add(on_sound_finished_idle, audio_stream_source);
}

CtmProcessPage* ctm_process_page_new(void) {
    CtmProcessPage* process_page = malloc(sizeof(CtmProcessPage));
    if (process_page == NULL) {
        fprintf(stderr, "ctm_process_page_new: malloc failed\n");
        return NULL;
    }

    // Tree view.
    GType types[G_N_ELEMENTS(PROCESS_PAGE_COLUMNS)];
    for (size_t i = 0; i < G_N_ELEMENTS(PROCESS_PAGE_COLUMNS); i++) {
        types[i] = PROCESS_PAGE_COLUMNS[i].type;
    }

    process_page->process_store = gtk_tree_store_newv(G_N_ELEMENTS(PROCESS_PAGE_COLUMNS), types);
    process_page->process_view  = gtk_tree_view_new_with_model(GTK_TREE_MODEL(process_page->process_store));
    GtkCellRenderer* renderer   = gtk_cell_renderer_text_new();

    for (size_t i = 0; i < sizeof(PROCESS_PAGE_COLUMNS) / sizeof(CtmProcessPageColumn); i++) {
        if (PROCESS_PAGE_COLUMNS[i].visible) {
            GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes(
                PROCESS_PAGE_COLUMNS[i].title,
                renderer,
                "text",
                PROCESS_PAGE_COLUMNS[i].column_id,
                NULL
            );
            gtk_tree_view_column_set_resizable(column, TRUE);
            gtk_tree_view_column_set_sort_column_id(column, PROCESS_PAGE_COLUMNS[i].column_id);
            gtk_tree_view_append_column(GTK_TREE_VIEW(process_page->process_view), column);
        }
    }

    // Scrolled window.
    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(process_page->process_view));

    // Footer
    GtkWidget* footer_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(footer_box), 5);

    // Checkbox to show processes from all users.
    GtkWidget* show_all_users_checkbox = gtk_check_button_new_with_label("Show processes from all users");
    gtk_box_pack_start(GTK_BOX(footer_box), show_all_users_checkbox, FALSE, FALSE, 0);

    // Buttons.
    GtkWidget* btn_box      = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    GtkWidget* end_task_btn = gtk_button_new_with_label("End Process");
    gtk_button_box_set_layout(GTK_BUTTON_BOX(btn_box), GTK_BUTTONBOX_END);
    gtk_container_add(GTK_CONTAINER(btn_box), end_task_btn);
    gtk_box_pack_end(GTK_BOX(footer_box), btn_box, FALSE, FALSE, 0);

    // Layout.
    process_page->widget = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(process_page->widget), scrolled_window, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(process_page->widget), footer_box, FALSE, FALSE, 5);

    process_page->include_processes_from_all_users = false;

    g_signal_connect(show_all_users_checkbox, "toggled", G_CALLBACK(toggle_show_processes_from_all_users), process_page);
    g_signal_connect(end_task_btn, "clicked", G_CALLBACK(ctm_process_page_end_task), process_page);

    return process_page;
}

GtkWidget* ctm_process_page_get_widget(const CtmProcessPage* self) {
    if (self == NULL) {
        fprintf(stderr, "%s: self cannot be NULL\n", __func__);
        return NULL;
    }
    return self->widget;
}

bool ctm_process_page_get_include_processes_from_all_users(const CtmProcessPage* self) {
    if (self == NULL) {
        fprintf(stderr, "%s: self cannot be NULL\n", __func__);
        return false;
    }
    return self->include_processes_from_all_users;
}

void ctm_process_page_set_include_processes_from_all_users(CtmProcessPage* self, bool value) {
    if (self == NULL) {
        fprintf(stderr, "%s: self cannot be NULL\n", __func__);
        return;
    }
    self->include_processes_from_all_users = value;
}

int ctm_process_page_refresh(const CtmProcessPage* self, const CtmProcessArray* processes) {
    assert(self != NULL);
    assert(processes != NULL);

    GtkTreeIter iter;

    // Mark all existing rows as removed.
    gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(self->process_store), &iter);
    while (valid) {
        gtk_tree_store_set(self->process_store, &iter, CTM_PROCESS_PAGE_UPDATED_COLUMN, FALSE, -1);
        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(self->process_store), &iter);
    }

    // Update all existing rows and create new rows.
    for (size_t i = 0; i < ctm_process_array_get_count(processes); i++) {
        const CtmProcess* process = ctm_process_array_get_element(processes, i);
        if (process == NULL) {
            fprintf(stderr, "ctm_process_page_refresh: process cannot be NULL\n");
            continue;
        }

        GtkTreeIter target_iter;
        const pid_t process_pid   = ctm_process_get_pid(process);
        const int   search_result = find_pid_in_process_store(process_pid, self->process_store, &target_iter); // Is the current process PID in the store?

        if (search_result == 0) {
            // We found an existing row. Update it.
            set_row_from_process(self->process_store, &target_iter, process);
        } else if (search_result == -1) {
            // This row does not exist. Create it.
            gtk_tree_store_append(self->process_store, &target_iter, NULL);
            set_row_from_process(self->process_store, &target_iter, process);
        }
    }

    // Remove rows which weren't updated or created.
    valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(self->process_store), &iter);
    while (valid) {
        gboolean updated;
        gtk_tree_model_get(GTK_TREE_MODEL(self->process_store), &iter,
                           CTM_PROCESS_PAGE_UPDATED_COLUMN, &updated,
                           -1);
        valid = updated ? gtk_tree_model_iter_next(GTK_TREE_MODEL(self->process_store), &iter) : gtk_tree_store_remove(self->process_store, &iter);
    }

    return 0;
}

void ctm_process_page_end_task(GtkButton* btn, gpointer self) {
    (void) btn;
    if (self == NULL) {
        fprintf(stderr, "on_end_task_btn_clicked: data cannot be NULL\n");
        return;
    }

    const CtmProcessPage* process_page = self;
    GtkTreeSelection*     selection    = gtk_tree_view_get_selection(GTK_TREE_VIEW(process_page->process_view));
    GtkTreeModel*         model;
    GtkTreeIter           iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gint   pid;
        gchar* name;

        gtk_tree_model_get(model, &iter, CTM_PROCESS_PAGE_PID_COLUMN, &pid, CTM_PROCESS_PAGE_NAME_COLUMN, &name, -1);

        if (pid > 1) {
            if (kill(pid, SIGKILL) == 0) {
                g_print("Killed task %s\n", name);
                ctm_sound_play_async("/home/martin/Nedlastinger/Wilhelm Scream Remastered.wav", sound_finish_bridge, NULL);
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
