//
// Created by martin on 30.03.2026.
//

#ifndef CLUELESS_TASK_MGR_CTM_COMMON_H
#define CLUELESS_TASK_MGR_CTM_COMMON_H

typedef struct CtmAppContext CtmAppContext;

// Constructor and destructor.
CtmAppContext* ctm_app_context_new(void);
void           ctm_app_context_free(CtmAppContext* ctx);

// Accessors.
GtkTreeStore* ctm_app_context_get_process_store(const CtmAppContext* ctx);
int           ctm_app_context_set_process_store(CtmAppContext* ctx, GtkTreeStore* store);
GtkTreeView*  ctm_app_context_get_process_view(const CtmAppContext* ctx);
int           ctm_app_context_set_process_view(CtmAppContext* ctx, GtkTreeView* view);
gboolean      ctm_app_context_get_show_processes_from_all_users(const CtmAppContext* ctx);
void          ctm_app_context_set_show_processes_from_all_users(CtmAppContext* ctx, gboolean value);

// Utilities.
void ctm_format_duration(long long time_in_ms, char* buffer, size_t buffer_size);

#endif //CLUELESS_TASK_MGR_CTM_COMMON_H
