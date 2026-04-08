#ifndef CTM_SAMPLER_H
#define CTM_SAMPLER_H
#include <stddef.h>

typedef struct Sampler Sampler;
typedef int (*         SamplerCaptureFunc)(void* slot);
typedef long (*        SamplerProcessFunc)(const void* current, const void* previous);
typedef int (*         SamplerCallback)(Sampler* sampler);

#define SAMPLER(ptr) ((Sampler*)(ptr))

Sampler* sampler_create(unsigned int interval_ms, size_t item_size, SamplerCaptureFunc capture_fn);
int      sampler_destroy(Sampler* sampler);
int      sampler_start(Sampler* sampler);
int      sampler_stop(Sampler* sampler);
int      sampler_subscribe(Sampler* sampler, SamplerCallback callback);
long     sampler_get_value(Sampler* sampler, size_t window, SamplerProcessFunc process_fn);

/**
 * Create a new sampler and start it.
 * @param interval_ms The sample interval in milliseconds.
 * @param item_size The size of the type of items to sample.
 * @param capture_fn The capturing function to use when sampling.
 * @param callback The callback function to use for subscribers.
 * @return The newly created and running sampler.
 */
Sampler* sampler_create_with_subscription_and_start(unsigned int       interval_ms,
                                                    size_t             item_size,
                                                    SamplerCaptureFunc capture_fn,
                                                    SamplerCallback    callback);

#endif
