#include "ctm/ctm.h"

#include <gtk/gtk.h>

#define CTM_APP_TITLE "Clueless Task Manager"
#define CTM_APP_WIDTH 800
#define CTM_APP_HEIGHT 600

typedef struct {
    GtkWidget* status_box;
    GtkWidget* first_column;
    GtkWidget* second_column;
    GtkWidget* third_column;

    Sampler*     cpu_sampler;
    ProcessList* process_list;
} AppContext;

AppContext g_ctx;

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
        gtk_label_set_text(GTK_LABEL(g_ctx.second_column), label_text);

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
    size_t process_count;

    (void) user_data;
    (void) frame_clock;
    (void) widget;

    if (g_ctx.process_list == NULL) {
        return G_SOURCE_CONTINUE;
    }

    if ((process_count = process_list_refresh(g_ctx.process_list)) > 0) {
        char label_text[32];
        snprintf(label_text, sizeof(label_text), "Processes: %lu", process_count);
        gtk_label_set_text(GTK_LABEL(g_ctx.first_column), label_text);
    }

    if (errno) {
        fprintf(stderr, "on_tick: Failed to refresh process list: %s\n", strerror(errno));
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
    GtkWidget* window;
    GtkWidget* vbox;
    GtkWidget* label;

    (void) user_data;

    g_ctx.cpu_sampler  = sampler_create_with_subscription_and_start(500, cpu_size(), (SamplerCaptureFunc) cpu_capture, (SamplerCallback) ui_update);
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
    label = gtk_label_new("Main Content Area");
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

    /* Status bar setup */
    g_ctx.status_box    = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    g_ctx.first_column  = gtk_label_new("Processes: 0");
    g_ctx.second_column = gtk_label_new("CPU Usage: 0%");
    g_ctx.third_column  = gtk_label_new("Memory Usage: 0%");

    gtk_box_pack_start(GTK_BOX(g_ctx.status_box), g_ctx.first_column, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(g_ctx.status_box), g_ctx.second_column, TRUE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(g_ctx.status_box), g_ctx.third_column, FALSE, FALSE, 5);

    gtk_box_pack_end(GTK_BOX(vbox), g_ctx.status_box, FALSE, FALSE, 0);

    gtk_widget_add_tick_callback(window, on_tick, NULL, NULL);

    gtk_widget_show_all(window);
}

int ui_start(int argc, char** argv) {
    GtkApplication* app;
    int             status;

    g_ctx.status_box    = NULL;
    g_ctx.first_column  = NULL;
    g_ctx.second_column = NULL;
    g_ctx.third_column  = NULL;
    g_ctx.cpu_sampler   = NULL;

    app = gtk_application_new("no.clueless.ctm", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
