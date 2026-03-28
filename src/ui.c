//
// Created by martin on 26.03.2026.
//

#include "ui.h"

#include <gtk/gtk.h>

#include "tabs/process_tab.h"
#include "tabs/performance_tab.h"

void ui_create_window(app_context_t *ctx) {
    // Create the window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Clueless Task Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

    // Create the notebook
    GtkWidget *notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(window), notebook);

    // Process tab
    GtkWidget *process_tab = process_tab_create(ctx);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), process_tab, gtk_label_new("Processes"));

    // Performance tab
    GtkWidget *performance_tab = performance_tab_create(ctx);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), performance_tab, gtk_label_new("Performance"));

    ctx->window = window;
}
