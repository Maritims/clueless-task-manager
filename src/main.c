//
// Created by martin on 26.03.2026.
//

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <gtk/gtk.h>

#include "ctm/ui.h"
#include "ctm/sys.h"
#include "ctm/sound.h"

void cui_format_duration(const long long time_in_ms, char* buffer, const size_t buffer_size) {
    if (buffer == NULL) {
        fprintf(stderr, "ctm_format_duration: buffer cannot be NULL\n");
        return;
    }
    if (buffer_size == 0) {
        fprintf(stderr, "ctm_format_duration: buffer_size cannot be 0\n");
        return;
    }

    const unsigned int hours   = time_in_ms == 0 ? 0 : (int) (time_in_ms / 3600000);
    const unsigned int minutes = time_in_ms == 0 ? 0 : (int) (time_in_ms % 3600000 / 60000);
    const unsigned int seconds = time_in_ms == 0 ? 0 : (int) (time_in_ms % 60000 / 1000);

    snprintf(buffer, buffer_size, "%02u:%02u:%02u", hours, minutes, seconds);
    buffer[buffer_size - 1] = '\0';
}

// region Event handlers
static gboolean on_refresh_timeout(gpointer data) {
    if (data == NULL) {
        fprintf(stderr, "on_refresh_timeout: data cannot be NULL\n");
        return FALSE;
    }

    const CtmAppContext*   ctx                              = data;
    const CtmProcessPage*  process_page                     = CtmAppContext_GetProcessPage(ctx);
    const CtmFooter*       footer                           = CtmAppContext_GetFooter(ctx);
    const bool             include_processes_from_all_users = CtmProcessPage_GetIncludeProcessesFromAllUsers(process_page);
    const CtmProcessArray* processes                        = CtmProcessArray_LoadFromKernel(include_processes_from_all_users);
    const size_t           process_count                    = CtmProcessArray_GetCount(processes);
    const CtmCpu*          cpu_stats                        = CtmCpu_LoadFromKernel();

    if (cpu_stats == NULL) {
        fprintf(stderr, "%s: Failed to fetch CPU stats\n", __func__);
        return FALSE;
    }

    const unsigned int cpu_usage_scaled = CtmCpu_GetUsageScaled(cpu_stats);

    CtmProcessPage_Refresh(process_page, processes);
    CtmFooter_Refresh(footer, process_count, cpu_usage_scaled);

    return TRUE;
}

// endregion

static void activate(GtkApplication* app, gpointer user_data) {
    (void) user_data;

    // Window.
    GtkWidget* window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Clueless Task Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
    gtk_window_set_application(GTK_WINDOW(window), app);

    // Status bar.
    CtmFooter* footer           = CtmFooter_Create();
    GtkWidget* footer_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget* footer_widget    = CtmFooter_GetWidget(footer);
    gtk_box_pack_start(GTK_BOX(footer_container), footer_widget, FALSE, FALSE, 0);

    // Pages.
    GtkWidget*      notebook            = gtk_notebook_new();
    CtmProcessPage* process_page        = CtmProcessPage_Create();
    GtkWidget*      process_page_widget = CtmProcessPage_GetWidget(process_page);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), process_page_widget, gtk_label_new("Processes"));

    // Layout container.
    GtkWidget* layout_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(layout_container), notebook, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(layout_container), footer_container, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(window), layout_container);

    // Context.
    CtmAppContext* ctx = CtmAppContext_Create(process_page, footer);
    assert(ctx != NULL);

    // Event handlers.
    guint on_refresh_timeout_id = g_timeout_add(1000, on_refresh_timeout, ctx);

    // Show everything.
    gtk_widget_show_all(window);
}

int main(int argc, char** argv) {
    ctm_sound_init();

    GtkApplication* app = gtk_application_new("no.clueless.task_manager", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    ctm_sound_deinit();

    return status;
}
