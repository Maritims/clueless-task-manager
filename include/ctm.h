#ifndef CTM_H
#define CTM_H

#define INSIDE_CTM_H

#include "ui/ctmappcontext.h"
#include "ui/ctmfooter.h"
#include "ui/ctmprocesspage.h"

// Utilities.
void ctm_format_duration(long long time_in_ms, char* buffer, size_t buffer_size);

#undef INSIDE_CTM_H
#endif