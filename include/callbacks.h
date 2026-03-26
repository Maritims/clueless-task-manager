//
// Created by martin on 26.03.2026.
//

#ifndef CLUELESS_TASK_MGR_CALLBACKS_H
#define CLUELESS_TASK_MGR_CALLBACKS_H
#include <gtk/gtk.h>

gboolean on_tick(gpointer data);

gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data);

#endif //CLUELESS_TASK_MGR_CALLBACKS_H
