//
// Created by martin on 26.03.2026.
//

#include "ui.h"
#include "callbacks.h"
#include "style.h"

#include <gtk/gtk.h>

#include "tabs/performance.h"

static GtkWidget *create_root_window(app_context_t *ctx) {
    ctx->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(ctx->window), "Clueless Task Manager");
    gtk_window_set_default_size(GTK_WINDOW(ctx->window), 640, 480);
    gtk_container_set_border_width(GTK_CONTAINER(ctx->window), 10);
    gtk_window_set_decorated(GTK_WINDOW(ctx->window), FALSE);
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

    style_add_class(ctx->drawing_area, "sunken");

    gtk_box_pack_start(GTK_BOX(vbox), ctx->drawing_area, TRUE, TRUE, 0);
    g_signal_connect(ctx->drawing_area, "draw", G_CALLBACK(on_draw), ctx);
    return vbox;
}

GtkWidget *ui_create_window(app_context_t *ctx) {
    create_root_window(ctx);
    GtkWidget *main_container = create_layout(ctx);

    // Configure notebook
    ctx->notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(main_container), ctx->notebook, TRUE, TRUE, 0);

    // Add Performance tab
    GtkWidget *performance_tab = performance_tab_create(ctx);
    gtk_notebook_append_page(GTK_NOTEBOOK(ctx->notebook), performance_tab, gtk_label_new("Performance"));

    // Add Processes tab
    gtk_notebook_append_page(GTK_NOTEBOOK(ctx->notebook), gtk_label_new("Processes"), NULL);

    return ctx->window;
}