//
// Created by martin on 02.04.2026.
//

#include <assert.h>
#include <gtk/gtk.h>
#include "ctm/ui.h"

struct CtmAppContext {
    CtmProcessPage* process_page;
    CtmFooter*      footer;
};

CtmAppContext* CtmAppContext_Create(CtmProcessPage* process_page, CtmFooter* footer) {
    assert(process_page != NULL);
    assert(footer != NULL);

    CtmAppContext* self = g_new0(CtmAppContext, 1);
    self->process_page  = process_page;
    self->footer        = footer;
    return self;
}

void CtmAppContext_Destroy(CtmAppContext* self) {
    if (self == NULL) {
        fprintf(stderr, "ctm_app_context_destroy: ctx cannot be NULL\n");
        return;
    }
    g_free(self);
}

CtmProcessPage* CtmAppContext_GetProcessPage(const CtmAppContext* self) {
    assert(self != NULL);
    assert(self->process_page != NULL);
    return self->process_page;
}

CtmFooter* CtmAppContext_GetFooter(const CtmAppContext* self) {
    assert(self != NULL);
    assert(self->footer != NULL);
    return self->footer;
}