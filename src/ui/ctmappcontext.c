//
// Created by martin on 02.04.2026.
//

#include <assert.h>
#include <gtk/gtk.h>
#include "ctm.h"

struct CtmAppContext {
    CtmProcessPage* process_page;
    CtmFooter*      footer;
};

CtmAppContext* ctm_app_context_new(CtmProcessPage* process_page, CtmFooter* footer) {
    assert(process_page != NULL);
    assert(footer != NULL);

    CtmAppContext* self = g_new0(CtmAppContext, 1);
    self->process_page  = process_page;
    self->footer        = footer;
    return self;
}

void ctm_app_context_free(CtmAppContext* self) {
    if (self == NULL) {
        fprintf(stderr, "ctm_app_context_destroy: ctx cannot be NULL\n");
        return;
    }
    g_free(self);
}

CtmProcessPage* ctm_app_context_get_process_page(const CtmAppContext* self) {
    assert(self != NULL);
    assert(self->process_page != NULL);
    return self->process_page;
}

CtmFooter* ctm_app_context_get_footer(const CtmAppContext* self) {
    assert(self != NULL);
    assert(self->footer != NULL);
    return self->footer;
}