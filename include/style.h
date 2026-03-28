//
// Created by martin on 27.03.2026.
//

#ifndef CLUELESS_TASK_MGR_STYLE_H
#define CLUELESS_TASK_MGR_STYLE_H
#include <gtk/gtk.h>

/**
 * Initializes the CSS provider and applies it to the default screen.
 */
void style_init(void);

/**
 * Helper to add a specific class to a widget.
 */
void style_add_class(GtkWidget *widget, const char *class_name);

#endif //CLUELESS_TASK_MGR_STYLE_H
