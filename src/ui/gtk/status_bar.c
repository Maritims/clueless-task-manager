#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include "ui.h"
#include "log/log.h"

struct StatusBar {
    GtkWidget* widget;
    GtkWidget* process_label;
    GtkWidget* cpu_label;
    GtkWidget* memory_label;
};

StatusBar* status_bar_create(void)
{
    StatusBar* status_bar = malloc(sizeof(StatusBar));
    if (status_bar == NULL) {
        fprintf(stderr, "status_bar_create: Failed to allocate memory for status bar: %s\n", strerror(errno));
        return NULL;
    }

    status_bar->widget        = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    status_bar->process_label = gtk_label_new("Processes: 0");
    status_bar->cpu_label     = gtk_label_new("CPU: 0%");
    status_bar->memory_label  = gtk_label_new("Memory: 0%");

    gtk_box_pack_start(GTK_BOX(status_bar->widget), status_bar->process_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(status_bar->widget), status_bar->cpu_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(status_bar->widget), status_bar->memory_label, FALSE, FALSE, 0);

    return status_bar;
}

void status_bar_destroy(StatusBar* status_bar)
{
    if (status_bar) {
        gtk_widget_destroy(status_bar->widget);
        free(status_bar);
    }
}

void status_bar_update(StatusBar*   status_bar,
                       const size_t process_count,
                       const long   cpu_usage,
                       const long   memory_usage)
{
    char memory_label_buf[16];

    snprintf(memory_label_buf, sizeof(memory_label_buf), "Memory: %.2f%%", (double) memory_usage / 1000.0);

    status_bar_set_process_count(status_bar, process_count);
    status_bar_set_cpu_usage(status_bar, cpu_usage);
    gtk_label_set_text(GTK_LABEL(status_bar->memory_label), memory_label_buf);
}

void status_bar_register(StatusBar* status_bar,
                         void*      status_bar_container)
{
    RETURN_VOID_IF_ARG_IS_NULL("status_bar_register", status_bar, "status_bar");
    RETURN_VOID_IF_ARG_IS_NULL("status_bar_register", status_bar_container, "status_bar_container");

    gtk_box_pack_end(GTK_BOX(status_bar_container), status_bar->widget, FALSE, FALSE, 0);
}

void status_bar_set_process_count(StatusBar*   status_bar,
                                  const size_t process_count)
{
    char process_label_buf[16];
    snprintf(process_label_buf, sizeof(process_label_buf), "Processes: %lu", process_count);
    gtk_label_set_text(GTK_LABEL(status_bar->process_label), process_label_buf);
}

void status_bar_set_cpu_usage(StatusBar* status_bar,
                              const long cpu_usage)
{
    char cpu_label_buf[16];
    snprintf(cpu_label_buf, sizeof(cpu_label_buf), "CPU: %.2f%%", (double) cpu_usage / 1000.0);
    gtk_label_set_text(GTK_LABEL(status_bar->cpu_label), cpu_label_buf);
}
