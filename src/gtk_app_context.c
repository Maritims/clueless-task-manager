#define CTM_LOG_THRESHOLD CTM_LOG_LEVEL_DEBUG

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "logging/include/logging.h"
#include "metrics/cpu.h"
#include "metrics/process_list.h"
#include "metrics/sampler.h"

struct AppContext {
    ProcessTab* process_tab;
    StatusBar*  status_bar;

    ProcessList* process_list;
    Sampler*     cpu_sampler;
    Sampler*     memory_sampler;

    size_t process_count;
    long   total_cpu_usage;
    long   total_memory_usage;
};

AppContext* app_context_create(void) {
    AppContext* ctx = malloc(sizeof(AppContext));
    if (ctx == NULL) {
        fprintf(stderr, "app_context_create: Failed to allocate memory for app context: %s\n", strerror(errno));
        return NULL;
    }

    if ((ctx->process_tab = process_tab_create()) == NULL) {
        LOG_ERROR("app_context_create", "Failed to create process tab");
        app_context_destroy(ctx);
        return NULL;
    }

    if ((ctx->status_bar = status_bar_create()) == NULL) {
        LOG_ERROR("app_context_create", "Failed to create status bar");
        app_context_destroy(ctx);
        return NULL;
    }

    if ((ctx->process_list = process_list_create()) == NULL) {
        LOG_ERROR("app_context_create", "Failed to create process list");
        app_context_destroy(ctx);
        return NULL;
    }

    /*if ((ctx->cpu_sampler = sampler_create_with_subscription_and_start(500, cpu_size(), (SamplerCaptureFunc) cpu_capture, (SamplerCallback) queue_ui_update)) == NULL) {
        LOG_ERROR("app_context_create", "Failed to create CPU sampler");
        app_context_destroy(ctx);
        return NULL;
    }*/

    ctx->memory_sampler = NULL;

    return ctx;
}

void app_context_destroy(AppContext* ctx) {
    LOG_DEBUG("app_context_destroy", "Destroying application context");

    if (ctx) {
        process_tab_destroy(ctx->process_tab);
        status_bar_destroy(ctx->status_bar);

        process_list_destroy(ctx->process_list);
        sampler_destroy(ctx->cpu_sampler);
        sampler_destroy(ctx->memory_sampler);

        free(ctx);
    }
}

void app_context_update(AppContext* ctx) {
    if (ctx == NULL) {
        LOG_ERROR("app_context_update", ("ctx cannot be NULL"));
        return;
    }

    ctx->total_cpu_usage    = 0; /*sampler_get_value(ctx->cpu_sampler, 10, (SamplerProcessFunc) cpu_get_total_usage);*/
    ctx->total_memory_usage = 0;
}

ProcessList* app_context_get_process_list(const AppContext* ctx) {
    if (ctx == NULL) {
        LOG_ERROR("app_context_get_process_list", ("ctx cannot be NULL"));
        return NULL;
    }

    if (ctx->process_list == NULL) {
        LOG_WARN("app_context_get_process_list", "Attempted to retrieve process list from context, but the process list was NULL");
    }
    return ctx->process_list;
}

size_t app_context_get_process_count(const AppContext* ctx) {
    if (ctx == NULL) {
        LOG_ERROR("app_context_get_process_count", "ctx cannot be NULL");
        return 0;
    }
    return ctx->process_count;
}

long app_context_get_cpu_usage(AppContext* ctx) {
    if (ctx == NULL) {
        LOG_ERROR("app_context_get_total_cpu_usage", "ctx cannot be NULL");
        return -1;
    }
    return ctx->total_cpu_usage;
}

long app_context_get_memory_usage(AppContext* ctx) {
    if (ctx == NULL) {
        LOG_ERROR("app_context_get_total_memory_usage", "ctx cannot be NULL");
        return -1;
    }
    return ctx->total_memory_usage;
}

ProcessTab* app_context_get_process_tab(AppContext* ctx) {
    if (ctx == NULL) {
        LOG_ERROR("app_context_get_process_tab", ("ctx cannot be NULL"));
        return NULL;
    }
    return ctx->process_tab;
}

StatusBar* app_context_get_status_bar(AppContext* ctx) {
    if (ctx == NULL) {
        LOG_ERROR("app_context_get_status_bar", ("ctx cannot be NULL"));
        return NULL;
    }
    return ctx->status_bar;
}
