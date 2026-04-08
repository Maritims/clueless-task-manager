#include "ctm/ctm.h"

#include <gtk/gtk.h>

#define CTM_APP_TITLE "Clueless Task Manager"
#define CTM_APP_WIDTH 800
#define CTM_APP_HEIGHT 600

typedef struct {
    GtkWidget*         status_bar;
    guint              context_id;
    CPU*               cpu;
    SamplerCaptureFunc cpu_capture_fn;
} AppContext;

static gboolean ui_refresh_callback(gpointer data) {
    AppContext* ctx = (AppContext *) data;
    CPU*        cpu;
    char        buffer[1024];

    if (ctx->cpu_capture_fn(ctx->cpu) != 0) {
        return TRUE;
    }
}

static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget*  window;
    GtkWidget*  vbox;
    GtkWidget*  label;
    AppContext* ctx;

    ctx = (AppContext *) user_data;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), CTM_APP_TITLE);
    gtk_window_set_default_size(GTK_WINDOW(window), CTM_APP_WIDTH, CTM_APP_HEIGHT);

    /* Main container */
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    /* Main UI content */
    label = gtk_label_new("Main Content Area");
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

    /* Status bar setup */
    ctx->status_bar = gtk_statusbar_new();
    ctx->context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(ctx->status_bar), "cpu_status");
    gtk_box_pack_end(GTK_BOX(vbox), ctx->status_bar, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}

int ui_update(void* user_data) {
    (void) user_data;
    return 0;
}

int ui_start(int argc, char** argv) {
    GtkApplication* app;
    AppContext      ctx;
    CPU*            cpu;
    int             status;

    ctx.cpu = cpu_alloc();

    app = gtk_application_new("no.clueless.ctm", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
