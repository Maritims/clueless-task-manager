#ifndef CTM_H
#define CTM_H
#define INSIDE_CTM_H

typedef enum { false, true } bool;

#include "collections/collections.h"
#include "ctm/cpu.h"
#include "ctm/memory.h"
#include "ctm/process.h"
#include "ctm/process_list.h"
#include "ctm/sampler.h"
#include "ctm/ui.h"

#define AS_CPU(ptr) ((CPU*)(ptr))
#define AS_MEMORY(ptr) ((Memory*)(ptr))

#undef INSIDE_CTM_H
#endif
