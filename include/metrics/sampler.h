#ifndef CTM_SAMPLER_H
#define CTM_SAMPLER_H

#include <stddef.h>

typedef struct sampler sampler_t;

typedef int (* sampler_capture_func_t)(void* slot);
typedef long (*sampler_process_func_t)(const void* current,
                                       const void* previous);
typedef int (*sampler_callback_t)(sampler_t* sampler);

#define SAMPLER(ptr) ((Sampler*)(ptr))

/* Constructor, destructor */
sampler_t* sampler_create(unsigned int           interval_ms,
                          size_t                 item_size,
                          sampler_capture_func_t capture_fn);

int sampler_destroy(sampler_t* sampler);

int sampler_start(sampler_t* sampler);

int sampler_stop(sampler_t* sampler);

int sampler_subscribe(sampler_t*         sampler,
                      sampler_callback_t callback);

long sampler_get_value(sampler_t*             sampler,
                       size_t                 window,
                       sampler_process_func_t process_fn);

sampler_t* sampler_create_with_subscription_and_start(unsigned int           interval_ms,
                                                      size_t                 item_size,
                                                      sampler_capture_func_t capture_fn,
                                                      sampler_callback_t     callback);

#endif
