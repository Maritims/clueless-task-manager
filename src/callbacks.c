//
// Created by martin on 26.03.2026.
//

#include "callbacks.h"

#include "sysinfo.h"
#include "ui.h"

static void update_history(app_context_t *ctx, const double new_load) {
    if (ctx->history_count < HISTORY_SIZE) {
        ctx->cpu_history[ctx->history_count++] = new_load;
    } else {
        // Shift left.
        for (int i = 0; i < HISTORY_SIZE - 1; i++) {
            ctx->cpu_history[i] = ctx->cpu_history[i + 1];
        }
        ctx->cpu_history[HISTORY_SIZE - 1] = new_load;
    }

    gtk_widget_queue_draw(ctx->drawing_area);
}

gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    app_context_t *ctx = (app_context_t*) data;
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);

    // Draw background
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    // Draw grid
    cairo_set_source_rgb(cr, 0, 0.3, 0);
    cairo_set_line_width(cr, 0.5);
    for (int i = 0; i < allocation.width; i += 20) {
        cairo_move_to(cr, i, 0);
        cairo_line_to(cr, i, allocation.height);
    }
    for (int i = 0; i < allocation.height; i+= 20) {
        cairo_move_to(cr, 0, i);
        cairo_line_to(cr, allocation.width, i);
    }
    cairo_stroke(cr);

    // Draw CPU line
    if (ctx->history_count < 2) {
        return FALSE;
    }

    cairo_set_source_rgb(cr, 0, 1, 0);
    cairo_set_line_width(cr, 2.0);

    const double step = (double) allocation.width / (HISTORY_SIZE - 1);

    for (int i = 0; i < ctx->history_count; i++) {
        double x = i * step;
        // Invert Y because 0 is the top of the screen.
        double y = allocation.height - (ctx->cpu_history[i] / 100.0 * allocation.height);

        if (i == 0) {
            cairo_move_to(cr, x, y);
        } else {
            cairo_line_to(cr, x, y);
        }
    }
    cairo_stroke(cr);

    return FALSE;
}

gboolean on_tick(gpointer data) {
    app_context_t *ctx = (app_context_t*) data;
    sysinfo_t stats;

    if (sys_stats_fetch_all(&stats) == STAT_SUCCESS) {
        const double cpu_load = cpu_calculate_load(&ctx->last_cpu_stats, &stats.cpu);

        // Update progress bars.
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(ctx->cpu_bar), cpu_load / 100.0);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(ctx->memory_bar), stats.memory.used_percent / 100.0);

        // Push to history graph.
        update_history(ctx, cpu_load);

        // Save current stats for next delta.
        ctx->last_cpu_stats = stats.cpu;
    }

    return TRUE; // Return TRUE to keep the g_timeout timer running.
}
