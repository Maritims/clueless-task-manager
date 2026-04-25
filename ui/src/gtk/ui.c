#include <gtk/gtk.h>

#include "ui.h"
#include "log/log.h"
#include "metrics/cpu.h"
#include "metrics/sampler.h"

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

static gboolean on_update_cpu_usage(gpointer user_data) {
    Sampler*   cpu_sampler = user_data;
    const long sample      = sampler_get_value(cpu_sampler, 10, (SamplerProcessFunc) cpu_get_total_usage);
    StatusBar* status_bar  = app_context_get_status_bar(g_ctx);
    status_bar_set_cpu_usage(status_bar, sample);
    return G_SOURCE_CONTINUE;
}

gboolean on_cpu_sampled(gpointer user_data) {
    g_idle_add(on_update_cpu_usage, user_data);
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
    g_timeout_add(1000, on_cpu_sampled, g_ctx);

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
