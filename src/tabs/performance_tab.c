//
// Created by martin on 27.03.2026.
//

#include <gtk/gtk.h>
#include "tabs/performance_tab.h"

#include "callbacks.h"
#include "style.h"

GtkWidget *performance_tab_create(app_context_t *ctx) {
    GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(container), 10);

    // CPU section
    gtk_box_pack_start(GTK_BOX(container), gtk_label_new("CPU Usage:"), FALSE, FALSE, 0);
    ctx->cpu_bar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(container), ctx->cpu_bar, FALSE, FALSE, 0);

    // Graph section
    ctx->drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(ctx->drawing_area, -1, 120);
    style_add_class(ctx->drawing_area, "sunken");
    g_signal_connect(ctx->drawing_area, "draw", G_CALLBACK(on_draw), ctx);
    gtk_box_pack_start(GTK_BOX(container), ctx->drawing_area, TRUE, TRUE, 0);

    // Memory section
    gtk_box_pack_start(GTK_BOX(container), gtk_label_new("Memory Usage:"), FALSE, FALSE, 0);
    ctx->memory_bar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(container), ctx->memory_bar, FALSE, FALSE, 0);

    return container;
}
