#include <gtk/gtk.h>
#include "../../libs/log/include/log/log.h"
#include "ui.h"
#include "collections/hash_map.h"

typedef struct {
    const char* name;
    GType       type;
    gboolean    visible;
    gboolean    expandable;
    float       alignment;
} ProcessColumn;

struct ProcessTab {
    GtkWidget*    container_widget;
    GtkListStore* store;
    GtkWidget*    view;
    HashMap*      pid_to_iter;
};

enum { COL_PID, COL_NAME, COL_STATE, COL_ACTIVE, NUM_COLS };

static const int    VISUAL_ORDER[]      = {COL_NAME, COL_PID};
static const size_t VISUAL_COLUMN_COUNT = sizeof(VISUAL_ORDER) / sizeof(VISUAL_ORDER[0]);

static void on_ui_process_added(Process* process, void* user_data) {
    const ProcessTab*    self = user_data;
    GtkTreeIter          iter;
    GtkTreePath*         path;
    GtkTreeRowReference* row_reference;
    const unsigned int   pid = process_get_pid(process);

    gtk_list_store_append(self->store, &iter);
    gtk_list_store_set(self->store, &iter,
                       COL_PID, pid,
                       COL_NAME, process_get_name(process),
                       COL_STATE, process_get_state(process),
                       -1);

    path          = gtk_tree_model_get_path(GTK_TREE_MODEL(self->store), &iter);
    row_reference = gtk_tree_row_reference_new(GTK_TREE_MODEL(self->store), path);
    hash_map_put(self->pid_to_iter, &pid, &row_reference);
    gtk_tree_path_free(path);
}

static void on_ui_process_updated(Process* process, void* user_data) {
    const ProcessTab*     context       = user_data;
    unsigned int          pid           = process_get_pid(process);
    GtkTreeRowReference** row_reference = hash_map_get(context->pid_to_iter, &pid);

    if (row_reference && gtk_tree_row_reference_valid(*row_reference)) {
        GtkTreeIter  iter;
        GtkTreePath* path = gtk_tree_row_reference_get_path(*row_reference);

        if (gtk_tree_model_get_iter(GTK_TREE_MODEL(context->store), &iter, path)) {
            gtk_list_store_set(context->store, &iter,
                               COL_NAME, process_get_name(process),
                               COL_STATE, process_get_state(process),
                               -1);
        }
        gtk_tree_path_free(path);
    }
}

static void on_ui_process_removed(Process* process, void* user_data) {
    const ProcessTab*     self          = user_data;
    const unsigned int    pid           = process_get_pid(process);
    GtkTreeRowReference** row_reference = hash_map_get(self->pid_to_iter, &pid);

    if (row_reference && gtk_tree_row_reference_valid(*row_reference)) {
        GtkTreeIter  iter;
        GtkTreePath* path = gtk_tree_row_reference_get_path(*row_reference);

        if (gtk_tree_model_get_iter(GTK_TREE_MODEL(self->store), &iter, path)) {
            gtk_list_store_remove(self->store, &iter);
        }

        gtk_tree_path_free(path);
        gtk_tree_row_reference_free(*row_reference);
        hash_map_remove(self->pid_to_iter, &pid);
    }
}

ProcessTab* process_tab_create(void) {
    const ProcessColumn columns[NUM_COLS] = {
        {"PID", G_TYPE_UINT, 1, 0, 1.0f},
        {"Image Name", G_TYPE_STRING, 1, 1, 0.0f},
        {"State", G_TYPE_STRING, 1, 0, 0.0f},
        {"Active", G_TYPE_BOOLEAN, 0, 0, 0.0f}
    };
    size_t      i;
    ProcessTab* process_tab;
    GtkWidget*  scrolled_window;
    GtkWidget*  well_frame;

    if ((process_tab = malloc(sizeof(ProcessTab))) == NULL) {
        LOG_ERROR("process_tab_create", "Failed to allocate memory for process tab");
        return NULL;
    }

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_window, 800, 600);

    well_frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(well_frame), GTK_SHADOW_ETCHED_IN);
    gtk_widget_set_margin_top(well_frame, 20); /* The Win2000 gap below tabs */
    gtk_widget_set_margin_bottom(well_frame, 20);
    gtk_widget_set_margin_start(well_frame, 20);
    gtk_widget_set_margin_end(well_frame, 20);

    process_tab->store            = gtk_list_store_new(NUM_COLS, columns[COL_PID].type, columns[COL_NAME].type, columns[COL_STATE].type, columns[COL_ACTIVE].type);
    process_tab->view             = gtk_tree_view_new_with_model(GTK_TREE_MODEL(process_tab->store));
    process_tab->container_widget = well_frame;


    gtk_container_add(GTK_CONTAINER(scrolled_window), process_tab->view);
    gtk_container_add(GTK_CONTAINER(well_frame), scrolled_window);

    for (i = 0; i < VISUAL_COLUMN_COUNT; i++) {
        const int     model_index       = VISUAL_ORDER[i];
        ProcessColumn column_definition = columns[model_index];

        if (column_definition.visible) {
            GtkCellRenderer*   renderer = gtk_cell_renderer_text_new();
            GtkTreeViewColumn* column   = gtk_tree_view_column_new_with_attributes(column_definition.name, renderer, "text", model_index, NULL);
            gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), TRUE);

            g_object_set(renderer, "xalign", column_definition.alignment, NULL);

            if (column_definition.expandable) {
                gtk_tree_view_column_set_expand(GTK_TREE_VIEW_COLUMN(column), TRUE);
            }

            gtk_tree_view_append_column(GTK_TREE_VIEW(process_tab->view), column);
        }
    }

    process_tab->pid_to_iter = hash_map_create(sizeof(unsigned int), sizeof(GtkTreeRowReference *), hash_int, hash_compare_int);

    return process_tab;
}

void process_tab_destroy(ProcessTab* process_tab) {
    if (process_tab) {
        gtk_widget_destroy(process_tab->view);
        hash_map_free(process_tab->pid_to_iter);
        free(process_tab);
    }
}

size_t process_tab_update(ProcessTab* self, ProcessList* process_list) {
    ProcessListObserver observer = {
        on_ui_process_added,
        on_ui_process_updated,
        on_ui_process_removed,
    };
    size_t count;

    RETURN_ZERO_IF_ARG_IS_NULL("process_tab_update", self, "process_tab");
    RETURN_ZERO_IF_ARG_IS_NULL("process_tab_update", process_list, "process_list");

    /* Stop listening to model changes temporarily while refreshing the process list. */
    g_object_freeze_notify(G_OBJECT(self->store));

    process_list_refresh(process_list, &observer, self, &count);

    /* Re-enable and trigger a single UI update. */
    g_object_thaw_notify(G_OBJECT(self->store));

    return process_list_get_count(process_list);
}

void process_tab_register(ProcessTab* process_tab, void* tab_container) {
    RETURN_VOID_IF_ARG_IS_NULL("process_tab_register", process_tab, "process_tab");
    RETURN_VOID_IF_ARG_IS_NULL("process_tab_register", tab_container, "tab_container");

    gtk_notebook_append_page(GTK_NOTEBOOK(tab_container), process_tab->container_widget, gtk_label_new("Processes"));
}
