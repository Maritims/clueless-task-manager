#ifndef CTM_MEMORY_H
#define CTM_MEMORY_H

#ifndef INSIDE_CTM_H
#error "Only ctm/ctm.h can be included directly!"
#endif

typedef struct Memory Memory;

Memory*       memory_get(void);
void          memory_free(Memory* memory);
unsigned long memory_get_total(const Memory* memory);
unsigned long memory_get_free(const Memory* memory);
unsigned long memory_get_available(const Memory* memory);

#endif
