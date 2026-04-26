#ifndef CTM_UI_H
#define CTM_UI_H
#include "process.h"
#include "process_tab.h"

typedef struct AppContext AppContext;
typedef struct StatusBar  StatusBar;

AppContext* app_context_create(void);
void        app_context_destroy(AppContext* ctx);
void        app_context_update(AppContext* ctx);

list_node_t* app_context_get_process_list(const AppContext* ctx);
long         app_context_get_cpu_usage(AppContext* ctx);
long         app_context_get_memory_usage(AppContext* ctx);

StatusBar*  app_context_get_status_bar(AppContext* ctx);
ProcessTab* app_context_get_process_tab(AppContext* ctx);

StatusBar* status_bar_create(void);
void       status_bar_destroy(StatusBar* status_bar);
void       status_bar_update(StatusBar* status_bar,
                       size_t           process_count,
                       long             cpu_usage,
                       long             memory_usage);
void status_bar_register(StatusBar* status_bar,
                         void*      status_bar_container);
void status_bar_set_process_count(StatusBar* status_bar,
                                  size_t     process_count);
void status_bar_set_cpu_usage(StatusBar* status_bar,
                              long       cpu_usage);

int ui_start(int    argc,
             char** argv);
int on_cpu_sampled(void* user_data);

#endif
