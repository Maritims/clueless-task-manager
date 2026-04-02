//
// Created by martin on 28.03.2026.
//

#ifndef CTMPROCESSPAGE_H
#define CTMPROCESSPAGE_H

#if !defined (CTM_H)
#error "Only ctm.h can be included directly"
#endif

#include <stdbool.h>

#include "ctm_process.h"

typedef enum {
    CTM_PROCESS_PAGE_PID_COLUMN,      // G_TYPE_INT
    CTM_PROCESS_PAGE_NAME_COLUMN,     // G_TYPE_STRING
    CTM_PROCESS_PAGE_STATUS_COLUMN,   // G_TYPE_STRING
    CTM_PROCESS_PAGE_CPU_COLUMN,      // G_TYPE_STRING
    CTM_PROCESS_PAGE_USERNAME_COLUMN, // G_TYPE_STRING
    CTM_PROCESS_PAGE_TIME_COLUMN,     //
    CTM_PROCESS_PAGE_UPDATED_COLUMN,  // G_TYPE_BOOLEAN
    CTM_PROCESS_PAGE_COLUMNS
} CtmProcessColumn;

typedef struct CtmProcessPage CtmProcessPage;

CtmProcessPage* ctm_process_page_new(void);
GtkWidget*      ctm_process_page_get_widget(const CtmProcessPage* page);
bool            ctm_process_page_get_include_processes_from_all_users(const CtmProcessPage* page);
void            ctm_process_page_set_include_processes_from_all_users(CtmProcessPage* page, bool value);

int  ctm_process_page_refresh(const CtmProcessPage* self, const CtmProcessArray* processes);
void ctm_process_page_end_task(GtkButton* btn, gpointer self);

#endif //CTMPROCESSPAGE_H
