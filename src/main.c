//
// Created by martin on 26.03.2026.
//

#include <stdio.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "cpu.h"
#include "ui.h"

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);
    ui_apply_styles();

    app_context_t *ctx = g_new0(app_context_t, 1);
    cpu_fetch_stats(&ctx->last_cpu_stats);

    // Build UI.
    GtkWidget *window = ui_create_window(ctx);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), ctx);

    // Start 1-second heartbeat.
    g_timeout_add(1000, on_tick, ctx);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
