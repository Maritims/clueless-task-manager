#include <gtk/gtk.h>

#include "ui.h"
#include "logging/include/logging.h"
#include "metrics/cpu.h"
#include "metrics/process_list.h"

#define CTM_APP_TITLE "Clueless Task Manager"
#define CTM_APP_WIDTH 800
#define CTM_APP_HEIGHT 600

typedef enum {
    UPDATE_STATUS_BAR_OK,
    UPDATE_STATUS_BAR_STOP,
    UPDATE_STATUS_BAR_SKIP,
    UPDATE_STATUS_BAR_ERROR
} UpdateStatusBarResult;

AppContext* g_ctx;

static gboolean on_ui_update(gpointer user_data) {
    StatusBar*   status_bar;
    ProcessList* process_list;
    size_t       process_count;
    long         total_cpu_usage;
    long         total_memory_usage;

    (void) user_data;

    process_list       = app_context_get_process_list(g_ctx);
    process_count      = process_list_get_count(process_list);
    total_cpu_usage    = app_context_get_cpu_usage(g_ctx);
    total_memory_usage = app_context_get_memory_usage(g_ctx);

    if ((status_bar = app_context_get_status_bar(g_ctx)) == NULL) {
        LOG_ERROR("on_ui_update", "status_bar cannot be NULL");
        return G_SOURCE_REMOVE;
    }

    status_bar_update(status_bar, process_count, total_cpu_usage, total_memory_usage);

    return G_SOURCE_CONTINUE;
}

gboolean queue_ui_update(gpointer user_data) {
    (void) user_data;
    g_idle_add(on_ui_update, NULL);
    return G_SOURCE_CONTINUE;
}

static gboolean on_update_process_tab(gpointer data) {
    AppContext*  ctx           = data;
    const size_t process_count = process_tab_update(app_context_get_process_tab(ctx), app_context_get_process_list(ctx));
    status_bar_set_process_count(app_context_get_status_bar(ctx), process_count);
    return G_SOURCE_CONTINUE;
}

static void destroy(const GtkWidget* window, gpointer user_data) {
    (void) window;
    (void) user_data;
    app_context_destroy(g_ctx);
}

static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget*  window;
    GtkWidget*  vbox;
    GtkWidget*  notebook;
    StatusBar*  status_bar;
    ProcessTab* process_tab;

    RETURN_VOID_IF_ARG_IS_NULL("activate", app, "app");

    (void) user_data;
    g_ctx = app_context_create();

    /* Create application window */
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), CTM_APP_TITLE);
    gtk_window_set_default_size(GTK_WINDOW(window), CTM_APP_WIDTH, CTM_APP_HEIGHT);
    gtk_container_set_border_width(GTK_CONTAINER(window), 0);

    /* Create vbox for layout containment */
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 0);

    /* Add tabs */
    notebook = gtk_notebook_new();
    gtk_container_set_border_width(GTK_CONTAINER(notebook), 10);

    if ((process_tab = app_context_get_process_tab(g_ctx)) == NULL) {
        LOG_ERROR("activate", "Failed to create process tab. Quitting.");
        g_application_quit(g_application_get_default());
        return;
    }
    process_tab_register(process_tab, notebook);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_box_pack_start(GTK_BOX(vbox), notebook, FALSE, FALSE, 0);

    /* Add status bar */
    status_bar = app_context_get_status_bar(g_ctx);
    status_bar_register(status_bar, vbox);

    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
    g_timeout_add(1000, on_update_process_tab, g_ctx);

    gtk_widget_show_all(window);
}

int ui_start(int argc, char** argv) {
    GtkApplication* app;
    int             status;

    app = gtk_application_new("no.clueless.ctm", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
