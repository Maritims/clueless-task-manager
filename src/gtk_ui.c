#include <gtk/gtk.h>

#include "metrics/cpu.h"
#include "metrics/process_list.h"
#include "metrics/sampler.h"

#define CTM_APP_TITLE "Clueless Task Manager"
#define CTM_APP_WIDTH 800
#define CTM_APP_HEIGHT 600

typedef struct {
    GtkListStore* model;
    GtkWidget*    view;
} ProcessTable;

typedef struct {
    const char* name;
    GType       type;
    gboolean    visible;
} ProcessTableColumn;

typedef struct {
    GtkWidget* widget;
    GtkWidget* process_label;
    GtkWidget* cpu_label;
    GtkWidget* memory_label;
} StatusBox;

typedef struct {
    ProcessTable process_table;
    StatusBox    status_box;

    Sampler*     cpu_sampler;
    ProcessList* process_list;
} AppContext;

enum { COL_PID, COL_NAME, COL_STATE, COL_ACTIVE, NUM_COLS };

AppContext g_ctx;

static void process_tab_create(ProcessTable* out_process_table) {
    const ProcessTableColumn process_table_entries[NUM_COLS] = {
        {"PID", G_TYPE_UINT, 1},
        {"Name", G_TYPE_STRING, 1},
        {"State", G_TYPE_STRING, 1},
        {"Active", G_TYPE_BOOLEAN, 1}
    };
    const size_t process_table_entry_count = sizeof(process_table_entries) / sizeof(process_table_entries[0]);
    size_t       i;

    out_process_table->model = gtk_list_store_new(NUM_COLS,
                                                  process_table_entries[COL_PID].type,
                                                  process_table_entries[COL_NAME].type,
                                                  process_table_entries[COL_STATE].type,
                                                  process_table_entries[COL_ACTIVE].type);
    out_process_table->view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(out_process_table->model));

    for (i = 0; i < process_table_entry_count; i++) {
        if (process_table_entries[i].visible) {
            gtk_tree_view_append_column(GTK_TREE_VIEW(out_process_table->view), gtk_tree_view_column_new_with_attributes(process_table_entries[i].name, gtk_cell_renderer_text_new(), "text", i, NULL));
        }
    }
}

static void status_box_create(StatusBox* status_box) {
    status_box->widget        = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    status_box->process_label = gtk_label_new("Processes: 0");
    status_box->cpu_label     = gtk_label_new("CPU Usage: 0%");
    status_box->memory_label  = gtk_label_new("Memory Usage: 0%");

    gtk_box_pack_start(GTK_BOX(status_box->widget), status_box->cpu_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(status_box->widget), status_box->process_label, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(status_box->widget), status_box->memory_label, FALSE, FALSE, 5);
}

static gboolean update_gtk_widgets(void* user_data) {
    long total_usage;

    (void) user_data;

    /* Stop if any of the samplers has been nullified. It means we're shutting down. */
    if (g_ctx.cpu_sampler == NULL) {
        return G_SOURCE_REMOVE;
    }

    if ((total_usage = sampler_get_value(g_ctx.cpu_sampler, 10, (SamplerProcessFunc) cpu_get_total_usage)) > 0) {
        char label_text[16];
        snprintf(label_text, sizeof(label_text), "CPU: %.1Lf%%", (long double) total_usage / 1000.0);
        gtk_label_set_text(GTK_LABEL(g_ctx.status_box.cpu_label), label_text);

        return G_SOURCE_REMOVE;
    }
    if (errno == EAGAIN) {
        return G_SOURCE_REMOVE;
    }

    fprintf(stderr, "ui_update: Failed to get CPU usage: %s\n", strerror(errno));
    return G_SOURCE_REMOVE;
}

/**
 * Bridges a Sampler with GTK.
 * @param user_data Ignored.
 * @return Always zero.
 */
int ui_update(void* user_data) {
    (void) user_data;
    g_idle_add(update_gtk_widgets, NULL);
    return 0;
}

static gboolean on_tick(GtkWidget* widget, GdkFrameClock* frame_clock, gpointer user_data) {
    size_t      process_count;
    char        label_text[32];
    GtkTreeIter iter;

    (void) user_data;
    (void) frame_clock;
    (void) widget;

    if (g_ctx.process_list == NULL) {
        return G_SOURCE_CONTINUE;
    }

    if (process_list_refresh(g_ctx.process_list, NULL, NULL, &process_count) != 0) {
        fprintf(stderr, "on_tick: Failed to refresh process list: %s\n", strerror(errno));
        return G_SOURCE_CONTINUE;
    }

    snprintf(label_text, sizeof(label_text), "Processes: %lu", process_count);
    gtk_label_set_text(GTK_LABEL(g_ctx.status_box.process_label), label_text);

    /* g_ctx.process_list decides what to keep in the model */
    /* only that which is in the process list should be kept in the model */


    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(g_ctx.process_table.model), &iter)) {
        /*Keep only that which is in the process list.*/

        /* Mark all records inactive */
        while (gtk_tree_model_iter_next(GTK_TREE_MODEL(g_ctx.process_table.model), &iter)) {
            gtk_list_store_set(g_ctx.process_table.model, &iter, 0, "1234", 1, "Name", 2, "Running", -1);
        }

        gtk_list_store_append(g_ctx.process_table.model, &iter);
        gtk_list_store_set(g_ctx.process_table.model, &iter, 3, 0, -1);
    }

    return G_SOURCE_CONTINUE;
}

static void on_window_destroy(const GtkWidget* window, gpointer user_data) {
    (void) window;
    (void) user_data;

    sampler_destroy(g_ctx.cpu_sampler);
    g_ctx.cpu_sampler = NULL;
}

static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window, *vbox, *notebook;

    (void) user_data;

    g_ctx.cpu_sampler = sampler_create_with_subscription_and_start(500, cpu_size(), (SamplerCaptureFunc) cpu_capture, (SamplerCallback) ui_update);
    if ((g_ctx.process_list = process_list_alloc()) == NULL) {
        fprintf(stderr, "Failed to allocate process list: %s\n", strerror(errno));
        return;
    }

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), CTM_APP_TITLE);
    gtk_window_set_default_size(GTK_WINDOW(window), CTM_APP_WIDTH, CTM_APP_HEIGHT);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    /* Main container */
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    /* Main UI content */
    process_tab_create(&g_ctx.process_table);
    status_box_create(&g_ctx.status_box);

    notebook = gtk_notebook_new();
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), g_ctx.process_table.view, gtk_label_new("Processes"));

    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(vbox), g_ctx.status_box.widget, FALSE, FALSE, 0);

    gtk_widget_add_tick_callback(window, on_tick, NULL, NULL);

    gtk_widget_show_all(window);
}

int ui_start(int argc, char** argv) {
    GtkApplication* app;
    int             status;

    g_ctx.cpu_sampler = NULL;

    app = gtk_application_new("no.clueless.ctm", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
