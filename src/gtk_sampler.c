#include <gtk/gtk.h>
#include "ctm/ctm.h"

struct Sampler {
    void*        history;
    size_t       item_size;
    unsigned int interval_ms;
    int          is_running;
    guint        timer_id;

    size_t          head;
    size_t          count;
    pthread_mutex_t lock;
    pthread_t       thread;

    SamplerCaptureFunc capture_fn;
    SamplerCallback    callback;
};
