//
// Created by martin on 02.04.2026.
//

#include <gtk/gtk.h>

#include "../../include/ctm/ui.h"

struct CtmFooterColumn {
    const char* label;
    const char* value;
};

struct CtmFooter {
    GtkWidget* widget;
    GtkWidget* process_count_widget;
    GtkWidget* cpu_usage_widget;
    GtkWidget* commit_charge_widget;
};

const struct CtmFooterColumn footer_columns[] = {
    {.label = "Processes", .value = "0"},
    {.label = "CPU Usage", .value = "0.00%"},
    {.label = "Commit Charge", .value = "0 / 0"},
};

CtmFooter* CtmFooter_Create(void) {
    CtmFooter* footer = malloc(sizeof(CtmFooter));
    if (footer == NULL) {
        fprintf(stderr, "%s: Failed to allocate memory for footer bar: %d\n", __func__, errno);
        return NULL;
    }

    footer->widget               = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    footer->process_count_widget = gtk_label_new("0");
    footer->cpu_usage_widget     = gtk_label_new("0.00%");
    footer->commit_charge_widget = gtk_label_new("0 / 0");
    GtkWidget* vertical_bar      = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    GtkWidget* process_count_label = gtk_label_new("Processes:");
    gtk_box_pack_start(GTK_BOX(footer->widget), process_count_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(footer->widget), footer->process_count_widget, FALSE, FALSE, 0);

    GtkWidget* cpu_usage_label = gtk_label_new("CPU Usage:");
    gtk_box_pack_start(GTK_BOX(footer->widget), cpu_usage_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(footer->widget), footer->cpu_usage_widget, FALSE, FALSE, 0);

    GtkWidget* commit_charge_label = gtk_label_new("Commit Charge:");
    gtk_box_pack_start(GTK_BOX(footer->widget), commit_charge_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(footer->widget), footer->commit_charge_widget, FALSE, FALSE, 0);

    return footer;
}

GtkWidget* CtmFooter_GetWidget(const CtmFooter* footer_bar) {
    if (footer_bar == NULL) {
        fprintf(stderr, "%s: footer_bar cannot be NULL\n", __func__);
        return NULL;
    }
    return footer_bar->widget;
}

GtkWidget* CtmFooter_GetProcessCountWidget(const CtmFooter* footer) {
    if (footer == NULL) {
        fprintf(stderr, "%s: footer_bar cannot be NULL\n", __func__);
        return NULL;
    }
    return footer->process_count_widget;
}

GtkWidget* CtmFooter_GetCpuUsageWidget(const CtmFooter* footer) {
    if (footer == NULL) {
        fprintf(stderr, "%s: footer_bar cannot be NULL\n", __func__);
        return NULL;
    }
    return footer->cpu_usage_widget;
}

GtkWidget* CtmFooter_GetCommitChargeWidget(const CtmFooter* footer) {
    if (footer == NULL) {
        fprintf(stderr, "%s: footer cannot be NULL\n", __func__);
        return NULL;
    }
    return footer->commit_charge_widget;
}

void CtmFooter_Refresh(const CtmFooter* footer, const unsigned int processes_count, const unsigned long cpu_usage_scaled) {
    if (footer == NULL) {
        fprintf(stderr, "%s: footer cannot be NULL\n", __func__);
        return;
    }
    if (cpu_usage_scaled > 10000) {
        fprintf(stderr, "%s: cpu_usage_scaled is out of range: %lu\n", __func__, cpu_usage_scaled);
        return;
    }

    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f%%", (double) cpu_usage_scaled / 100.0);
    gtk_label_set_label(GTK_LABEL(footer->cpu_usage_widget), buf);
    buf[sizeof(buf) - 1] = '\0';

    snprintf(buf, sizeof(buf), "%u", processes_count);
    gtk_label_set_label(GTK_LABEL(footer->process_count_widget), buf);
    buf[sizeof(buf) - 1] = '\0';

    gtk_label_set_label(GTK_LABEL(footer->commit_charge_widget), "0 / 0");
}
