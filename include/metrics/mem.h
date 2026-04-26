#ifndef METRICS_MEM_H
#define METRICS_MEM_H

typedef struct mem mem_t;

typedef enum {
    MEM_SUCCESS,
    MEM_ERR_INVALID_ARG,
    MEM_ERR_OPEN,
    MEM_ERR_READ,
    MEM_ERR_PARSE,
    MEM_ERR_INTERNAL
} mem_result_t;

const char* mem_strerror(mem_result_t result);

mem_result_t mem_read(mem_t* out);

mem_result_t mem_destroy(mem_t* mem);

mem_result_t mem_total(const mem_t*   mem,
                       unsigned long* out);

mem_result_t mem_free(const mem_t*   mem,
                      unsigned long* out);

mem_result_t mem_available(const mem_t*   mem,
                           unsigned long* out);

#endif
