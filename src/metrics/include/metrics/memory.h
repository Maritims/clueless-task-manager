#ifndef MEM_METRICS_H
#define MEM_METRICS_H

typedef struct Memory Memory;

Memory*       memory_get(void);
void          memory_free(Memory* memory);
unsigned long memory_get_total(const Memory* memory);
unsigned long memory_get_free(const Memory* memory);
unsigned long memory_get_available(const Memory* memory);

#endif
