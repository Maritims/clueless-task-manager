//
// Created by martin on 27.03.2026.
//

#include "style.h"

void style_init(void) {
    GtkCssProvider *provider = gtk_css_provider_new();

    const char *css =
            "window { background-color: #C0C0C0; }"
            "label { color: black; font-family: 'MS Sans Serif', Arial; font-weight: bold; }"

            /* The "sunken" look for graphs and progress bars. */
            ".sunken {"
            "   background-color: #000000; "
            "   border-style: solid; "
            "   border-width: 2px; "
            "   border-color: #404040 #ffffff #ffffff #404040; "
            "}"

            /* Progress bar blocks */
            "progressbar progress {"
            "   background-color: #000080; "
            "   border-radius: 0; "
            " margin: 1px; "
            "}"
            "progressbar trough {"
            "   background-color: #808080; "
            "   border: 2px solid; "
            "   border-color: #808080 #ffffff #ffffff #808080; "
            "}";

    gtk_css_provider_load_from_data(provider, css, -1, NULL);

    gtk_style_context_add_provider_for_screen(
            gdk_screen_get_default(),
            GTK_STYLE_PROVIDER(provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_object_unref(provider);
}


void style_add_class(GtkWidget *widget, const char *class_name) {
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_class(context, class_name);
}