#ifndef METRICS_PROCESS_H
#define METRICS_PROCESS_H
#include <stddef.h>

#include "list.h"

typedef struct process       process_t;
typedef struct process_state process_state_t;

typedef void (*process_callback_t)(process_t* process);

typedef enum {
    PROCESS_SUCCESS,
    PROCESS_ERR_INVALID_ARG,
    PROCESS_ERR_NOT_FOUND,
    PROCESS_ERR_OPEN,
    PROCESS_ERR_READ,
    PROCESS_ERR_PARSE,
    PROCESS_ERR_INTERNAL
} process_result_t;

/* Utilities */
const char* process_strerror(process_result_t result);

/* Constructor, destructor, sizeof */
process_result_t process_read(unsigned int pid,
                              process_t*   out);

process_result_t process_destroy(process_t* process);

process_result_t process_sizeof(size_t* out);

process_result_t process_from_node(list_node_t* node,
                                   process_t**  out);

/* Accessors */
process_result_t process_pid(const process_t* process,
                             unsigned int*    out);

process_result_t process_name(const process_t* process,
                              char*            out,
                              size_t           len);

process_result_t process_state(const process_t* process,
                               char*            out,
                               size_t           len);

process_result_t process_user(const process_t* process,
                              char*            out,
                              size_t           len);

process_result_t process_active(const process_t* process,
                                int*             out);

process_result_t process_activate(process_t* process);

process_result_t process_deactivate(process_t* process);

process_result_t process_total_time(const process_t* process,
                                    unsigned int*    out);

process_result_t process_list_refresh(list_node_t*       list,
                                      size_t             len,
                                      process_callback_t on_process_added,
                                      process_callback_t on_process_updated,
                                      process_callback_t on_process_removed);

#endif
