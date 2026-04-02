//
// Created by martin on 02.04.2026.
//

#ifndef CTM_FOOTER_H
#define CTM_FOOTER_H

#if !defined (CTM_H)
#error "Only ctm.h can be included directly"
#endif

#include <gtk/gtk.h>

typedef struct CtmFooter CtmFooter;

CtmFooter* ctm_footer_new(void);
GtkWidget* ctm_footer_get_widget(const CtmFooter* footer_bar);
GtkWidget* ctm_footer_get_process_count_widget(const CtmFooter* footer);
GtkWidget* ctm_footer_get_cpu_usage_widget(const CtmFooter* footer);
GtkWidget* ctm_footer_get_commit_charge_widget(const CtmFooter* footer);
void       ctm_footer_refresh(const CtmFooter* footer, unsigned int processes_count, unsigned long cpu_usage_scaled);

#endif //CTM_FOOTER_H
