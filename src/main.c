//
// Created by martin on 26.03.2026.
//

#include <stdio.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "../modules/sysinfo/include/cpu.h"
#include "ui.h"
#include "style.h"

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);
    style_init();

    app_context_t *ctx = g_new0(app_context_t, 1);
    cpu_fetch_stats(&ctx->last_cpu_stats);
    sysinfo_fetch(&ctx->sysinfo);

    // Build UI.
    ui_create_window(ctx);
    g_signal_connect(ctx->window, "destroy", G_CALLBACK(gtk_main_quit), ctx);

    // Start 1-second heartbeat.
    g_timeout_add(1000, on_tick, ctx);

    gtk_widget_show_all(ctx->window);
    gtk_main();

    return 0;
}
