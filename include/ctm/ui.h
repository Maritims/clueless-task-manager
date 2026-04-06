#ifndef CTM_H
#define CTM_H

#include "ctm/process.h"

typedef struct CtmAppContext  CtmAppContext;
typedef struct CtmFooter      CtmFooter;
typedef struct CtmProcessPage CtmProcessPage;

typedef enum {
    CTM_PROCESS_PAGE_PID_COLUMN,      /* G_TYPE_INT */
    CTM_PROCESS_PAGE_NAME_COLUMN,     /* G_TYPE_STRING */
    CTM_PROCESS_PAGE_STATUS_COLUMN,   /* G_TYPE_STRING */
    CTM_PROCESS_PAGE_CPU_COLUMN,      /* G_TYPE_STRING */
    CTM_PROCESS_PAGE_USERNAME_COLUMN, /* G_TYPE_STRING */
    CTM_PROCESS_PAGE_TIME_COLUMN,     /* */
    CTM_PROCESS_PAGE_UPDATED_COLUMN,  /* G_TYPE_BOOLEAN */
    CTM_PROCESS_PAGE_COLUMNS
} CtmProcessColumn;

CtmAppContext*  CtmAppContext_Create(CtmProcessPage* process_page, CtmFooter* footer);
void            CtmAppContext_Destroy(CtmAppContext* self);
CtmProcessPage* CtmAppContext_GetProcessPage(const CtmAppContext* self);
CtmFooter*      CtmAppContext_GetFooter(const CtmAppContext* self);

CtmFooter* CtmFooter_Create(void);
GtkWidget* CtmFooter_GetWidget(const CtmFooter* footer_bar);
GtkWidget* CtmFooter_GetProcessCountWidget(const CtmFooter* footer);
GtkWidget* CtmFooter_GetCpuUsageWidget(const CtmFooter* footer);
GtkWidget* CtmFooter_GetCommitChargeWidget(const CtmFooter* footer);
void       CtmFooter_Refresh(const CtmFooter* footer, unsigned int processes_count, unsigned long cpu_usage_scaled);

CtmProcessPage* CtmProcessPage_Create(void);
GtkWidget*      CtmProcessPage_GetWidget(const CtmProcessPage* self);
int             CtmProcessPage_GetIncludeProcessesFromAllUsers(const CtmProcessPage* self);
void            CtmProcessPage_SetIncludeProcessesFromAllUsers(CtmProcessPage* page, int value);
int             CtmProcessPage_Refresh(const CtmProcessPage* self, const ctm_process_metrics_t* processes);
void            CtmProcessPage_EndTask(GtkButton* btn, gpointer self);

void cui_format_duration(unsigned long time_in_ms, char* buffer, size_t buffer_size);

#undef INSIDE_CTM_H
#endif
