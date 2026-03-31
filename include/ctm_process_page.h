//
// Created by martin on 28.03.2026.
//

#ifndef CTM_PROCESS_TAB_H
#define CTM_PROCESS_TAB_H

#include "ctm_common.h"
#include "ctm_process.h"

typedef enum {
    CTM_PROCESS_PAGE_PID_COLUMN,      // 0: G_TYPE_INT
    CTM_PROCESS_PAGE_NAME_COLUMN,     // 1: G_TYPE_STRING
    CTM_PROCESS_PAGE_STATUS_COLUMN,   // 2: G_TYPE_STRING
    CTM_PROCESS_PAGE_USERNAME_COLUMN, // 3: G_TYPE_STRING
    CTM_PROCESS_PAGE_UPDATED_COLUMN,  // 4: G_TYPE_BOOLEAN
    CTM_PROCESS_PAGE_COLUMNS          // 5
} CtmProcessColumn;

// Constructor.
GtkWidget* ctm_process_page_new(CtmAppContext* ctx);

// Logic.
int  ctm_process_page_refresh(const CtmAppContext* ctx);
void ctm_process_page_end_task(GtkButton* btn, gpointer data);

#endif //CTM_PROCESS_TAB_H
