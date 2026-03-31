//
// Created by martin on 26.03.2026.
//

#include <stdio.h>
#include <gtk/gtk.h>

#include "ctm_common.h"
#include "ctm_process_page.h"

struct CtmAppContext {
    GtkTreeStore *process_store;
    GtkTreeView  *process_view;
    gboolean     show_processes_from_all_users;
};

CtmAppContext *ctm_app_context_new(void) {
    return g_new0(CtmAppContext, 1);
}

void ctm_app_context_free(CtmAppContext *ctx) {
    if (ctx == NULL) {
        fprintf(stderr, "ctm_app_context_destroy: ctx cannot be NULL\n");
        return;
    }
}

GtkTreeStore *ctm_app_context_get_process_store(const CtmAppContext *ctx) {
    if (ctx == NULL) {
        fprintf(stderr, "ctm_app_context_get_process_store: ctx cannot be NULL\n");
        return NULL;
    }

    return ctx->process_store;
}

int ctm_app_context_set_process_store(CtmAppContext *ctx, GtkTreeStore *store) {
    if (ctx == NULL) {
        fprintf(stderr, "ctm_app_context_set_process_store: ctx cannot be NULL\n");
        return errno = EINVAL;
    }
    if (store == NULL) {
        fprintf(stderr, "ctm_app_context_set_process_store: store cannot be NULL\n");
        return errno = EINVAL;
    }

    ctx->process_store = store;
    return 0;
}

GtkTreeView *ctm_app_context_get_process_view(const CtmAppContext *ctx) {
    if (ctx == NULL) {
        fprintf(stderr, "ctm_process_page_get_process_view: ctx cannot be NULL\n");
        return NULL;
    }

    return ctx->process_view;
}

int ctm_app_context_set_process_view(CtmAppContext *ctx, GtkTreeView *view) {
    if (ctx == NULL) {
        fprintf(stderr, "ctm_process_page_set_process_view: ctx cannot be NULL\n");
        return errno = EINVAL;
    }
    if (view == NULL) {
        fprintf(stderr, "ctm_process_page_set_process_view: view cannot be NULL\n");
        return errno = EINVAL;
    }

    ctx->process_view = view;
    return 0;
}

gboolean ctm_app_context_get_show_processes_from_all_users(const CtmAppContext *ctx) {
    if (ctx == NULL) {
        fprintf(stderr, "ctm_app_context_get_show_processes_from_all_users: ctx cannot be NULL\n");
        return FALSE;
    }

    return ctx->show_processes_from_all_users;
}

void ctm_app_context_set_show_processes_from_all_users(CtmAppContext *ctx, const gboolean value) {
    if (ctx == NULL) {
        fprintf(stderr, "ctm_app_context_set_show_processes_from_all_users: ctx cannot be NULL\n");
        return;
    }

    ctx->show_processes_from_all_users = value;
}

// region Event handlers
static gboolean on_refresh_timeout(gpointer data) {
    if (data == NULL) {
        fprintf(stderr, "on_refresh_timeout: data cannot be NULL\n");
        return FALSE;
    }

    CtmAppContext *ctx = data;

    ctm_process_page_refresh(ctx);

    return TRUE;
}

// endregion

static void activate(GtkApplication *app, gpointer user_data) {
    (void) user_data;

    CtmAppContext *app_context = ctm_app_context_new();
    if (app_context == NULL) {
        fprintf(stderr, "Failed to create app context\n");
        return;
    }

    // Window.
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Clueless Task Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
    gtk_window_set_application(GTK_WINDOW(window), app);

    // Pages.
    GtkWidget *notebook     = gtk_notebook_new();
    GtkWidget *process_page = ctm_process_page_new(app_context);

    if (process_page == NULL) {
        fprintf(stderr, "Failed to create process page\n");
        return;
    }

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), process_page, gtk_label_new("Processes"));
    gtk_container_add(GTK_CONTAINER(window), notebook);

    // Event handlers.
    guint on_refresh_timeout_id = g_timeout_add(1000, on_refresh_timeout, app_context);

    // Show everything.
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("no.clueless.task_manager", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
