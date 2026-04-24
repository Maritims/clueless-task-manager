/**
 * @file sampler.h
 * @brief Generic engine for periodic data sampling and processing.
 *
 * Provides a framework for capturing snapshots of system data at fixed intervals
 * and calculating values based on the delta between samples.
 */

#ifndef CTM_SAMPLER_H
#define CTM_SAMPLER_H

#include <stddef.h>

/**
 * @brief Opaque handle for a sampler instance.
 */
typedef struct Sampler Sampler;

/**
 * @brief Callback to capture a single data snapshot.
 * @param slot Pointer to the memory location where the sample should be stored.
 * @retval 0 on success.
 * @retval Non-zero error code on failure.
 */
typedef int (*SamplerCaptureFunc)(void* slot);

/**
 * @brief Callback to process/calculate a value from two snapshots.
 * @param current The most recent data sample.
 * @param previous The previous data sample in the window.
 * @return The calculated value (e.g., usage percentage).
 */
typedef long (*SamplerProcessFunc)(const void* current, const void* previous);

/**
 * @brief Callback triggered after a successful sample capture.
 * @param sampler The sampler instance that triggered the callback.
 * @retval 0 to continue sampling.
 * @retval Non-zero to request the sampler to stop.
 */
typedef int (*SamplerCallback)(Sampler* sampler);

/**
 * @brief Casts a pointer to a Sampler type.
 */
#define SAMPLER(ptr) ((Sampler*)(ptr))

/**
 * @brief Initializes a new sampler instance.
 * @param interval_ms Frequency of sampling in milliseconds.
 * @param item_size Size in bytes of the data structure being sampled.
 * @param capture_fn Function used to populate the sample data.
 * @retval Pointer to the new Sampler instance on success.
 * @retval NULL if allocation or initialization fails.
 */
Sampler* sampler_create(unsigned int interval_ms, size_t item_size, SamplerCaptureFunc capture_fn);

/**
 * @brief Stops the sampler and releases all associated resources.
 * @param sampler The instance to destroy.
 * @retval 0 on success.
 * @retval Non-zero if destruction fails.
 */
int sampler_destroy(Sampler* sampler);

/**
 * @brief Starts the periodic sampling loop.
 * @details This typically spawns or utilizes a background thread.
 * @param sampler The sampler instance to start.
 * @retval 0 on success.
 * @retval Non-zero if the sampler could not be started.
 */
int sampler_start(Sampler* sampler);

/**
 * @brief Suspends the sampling loop.
 * @param sampler The sampler instance to stop.
 * @retval 0 on success.
 * @retval Non-zero if the sampler could not be stopped.
 */
int sampler_stop(Sampler* sampler);

/**
 * @brief Attaches a subscriber callback to the sampler.
 * @param sampler The sampler to observe.
 * @param callback The function to call after every successful capture.
 * @retval 0 on success.
 * @retval Non-zero on failure.
 */
int sampler_subscribe(Sampler* sampler, SamplerCallback callback);

/**
 * @brief Processes the sampled data to retrieve a calculated value.
 * @param sampler The sampler instance.
 * @param window The number of samples to look back (usually 1 for immediate delta).
 * @param process_fn Function used to perform the calculation between snapshots.
 * @return The result of the processing function.
 */
long sampler_get_value(Sampler* sampler, size_t window, SamplerProcessFunc process_fn);

/**
 * @brief Convenience function to create, configure, and start a sampler in one call.
 * @param interval_ms The sample interval in milliseconds.
 * @param item_size The size of the type of items to sample.
 * @param capture_fn The capturing function to use when sampling.
 * @param callback The callback function to use for subscribers.
 * @retval Pointer to the newly created and running sampler on success.
 * @retval NULL on failure.
 */
Sampler* sampler_create_with_subscription_and_start(unsigned int       interval_ms,
                                                    size_t             item_size,
                                                    SamplerCaptureFunc capture_fn,
                                                    SamplerCallback    callback);

#endif
