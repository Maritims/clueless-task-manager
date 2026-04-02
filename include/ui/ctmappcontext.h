//
// Created by martin on 02.04.2026.
//
#ifndef CTMAPPCONTEXT_H
#define CTMAPPCONTEXT_H

#if !defined (CTM_H)
#error "Only ctm.h can be included directly"
#endif

#include "ctmfooter.h"
#include "ctmprocesspage.h"

typedef struct CtmAppContext CtmAppContext;

// Constructor and destructor.
CtmAppContext* ctm_app_context_new(CtmProcessPage* process_page, CtmFooter* footer);
void           ctm_app_context_free(CtmAppContext* self);

// Accessors.
CtmProcessPage* ctm_app_context_get_process_page(const CtmAppContext* self);
CtmFooter*      ctm_app_context_get_footer(const CtmAppContext* self);

#endif // CTMAPPCONTEXT_H
