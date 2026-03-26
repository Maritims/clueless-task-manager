//
// Created by martin on 26.03.2026.
//

#include "ui.h"

#include <gtk/gtk.h>

#include "callbacks.h"

static GtkWidget *create_root_window(app_context_t *ctx) {
    ctx->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(ctx->window), "Clueless Task Manager");
    gtk_window_set_default_size(GTK_WINDOW(ctx->window), 300, 200);
    gtk_container_set_border_width(GTK_CONTAINER(ctx->window), 10);
    return ctx->window;
}

static GtkWidget *create_layout(app_context_t *ctx) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(ctx->window), vbox);
    return vbox;
}

static GtkWidget *create_cpu_section(app_context_t *ctx, GtkWidget *vbox) {
    gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new("CPU Usage:"), FALSE, FALSE, 0);
    ctx->cpu_bar = gtk_progress_bar_new();
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(ctx->cpu_bar), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), ctx->cpu_bar, FALSE, FALSE, 0);
    return vbox;
}

static GtkWidget *create_memory_section(app_context_t *ctx, GtkWidget *vbox) {
    gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new("Memory Usage:"), FALSE, FALSE, 0);
    ctx->memory_bar = gtk_progress_bar_new();
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(ctx->memory_bar), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), ctx->memory_bar, FALSE, FALSE, 0);
    return vbox;
}

static GtkWidget *create_graph_section(app_context_t *ctx, GtkWidget *vbox) {
    ctx->drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(ctx->drawing_area, -1, 100);
    gtk_box_pack_start(GTK_BOX(vbox), ctx->drawing_area, TRUE, TRUE, 0);
    g_signal_connect(ctx->drawing_area, "draw", G_CALLBACK(on_draw), ctx);
    return vbox;
}

GtkWidget *ui_create_window(app_context_t *ctx) {
    create_root_window(ctx);

    GtkWidget *vbox = create_layout(ctx);

    create_cpu_section(ctx, vbox);
    create_memory_section(ctx, vbox);
    create_graph_section(ctx, vbox);

    return ctx->window;
}

void ui_apply_styles() {
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css =
        "window { background-color: #008080; }" /* Classic Teal */
        "label { color: white; font-family: 'MS Sans Serif', Arial; font-weight: bold; }"
        "progressbar trough { background-color: #808080; border: 2px solid; border-color: #404040 #ffffff #ffffff #404040; }"
        "progressbar progress { background-color: #000080; border-radius: 0; }"; /* Navy Blue progress */

    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);
}