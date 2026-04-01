//
// Created by martin on 01.04.2026.
//

#include <math.h>
#include <sndfile.h>
#include <stdio.h>
#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>
#include <spa/utils/ringbuffer.h>
#include <sys/stat.h>

#define BUFFER_SIZE (128*1024)

typedef struct CtmAudioStreamSource {
    struct pw_thread_loop* thread_loop; // Manages timing and OS signals.
    struct pw_loop*        loop;
    SNDFILE*               input_snd_file_handle;  // The file to play.
    SF_INFO                input_snd_file_info;    // Metadata such as rate, channels, etc.
    struct pw_stream*      output_playback_stream; // The actual exit point for audio data.
    int                    eventfd;
    bool                   running;

    // Ring buffer state.
    struct spa_ringbuffer ring;
    int8_t                buffer[BUFFER_SIZE];
} CtmAudioStreamSource;

static void on_playback_process(void* userdata) {
    CtmAudioStreamSource* audio_stream_source  = userdata;
    struct pw_buffer*     stream_buffer_handle = pw_stream_dequeue_buffer(audio_stream_source->output_playback_stream);
    if (stream_buffer_handle == NULL) {
        fprintf(stderr, "%s: stream: Failed to dequeue buffer: %s\n", __func__, strerror(errno));
        return;
    }

    const struct spa_buffer* raw_audio_buffer       = stream_buffer_handle->buffer;
    int16_t*                 outbound_audio_payload = raw_audio_buffer->datas[0].data; // This is the water which we're looking for in the bucket.
    if (outbound_audio_payload == NULL) {
        fprintf(stderr, "%s: stream: Failed to get outbound audio payload\n", __func__);
        return;
    }

    const int      bytes_per_audio_frame = sizeof(int16_t) * audio_stream_source->input_snd_file_info.channels; // Our "step length", also known as "stride".
    const uint32_t required_bytes        = raw_audio_buffer->datas[0].maxsize;
    uint32_t       index;
    const uint32_t readable_bytes = spa_ringbuffer_get_read_index(&audio_stream_source->ring, &index);
    const uint32_t bytes_to_copy  = readable_bytes < required_bytes ? readable_bytes : required_bytes;

    if (bytes_to_copy > 0) {
        spa_ringbuffer_read_data(&audio_stream_source->ring, audio_stream_source->buffer, BUFFER_SIZE, index % BUFFER_SIZE, outbound_audio_payload, bytes_to_copy);
        spa_ringbuffer_read_update(&audio_stream_source->ring, index + bytes_to_copy);

        spa_system_eventfd_write(audio_stream_source->loop->system, audio_stream_source->eventfd, 1);
    }

    raw_audio_buffer->datas[0].chunk->offset = 0;
    raw_audio_buffer->datas[0].chunk->stride = bytes_per_audio_frame;
    raw_audio_buffer->datas[0].chunk->size   = bytes_to_copy;

    // Ship the bucket back to the system.
    pw_stream_queue_buffer(audio_stream_source->output_playback_stream, stream_buffer_handle);
}

static const struct pw_stream_events stream_events = {
    PW_VERSION_STREAM_EVENTS,
    .process = on_playback_process,
};

static void refill_ringbuffer(void* userdata) {
    CtmAudioStreamSource* audio_stream_source = userdata;
    const int             stride              = sizeof(int16_t) * audio_stream_source->input_snd_file_info.channels;
    int16_t               buffer[4096];
    uint32_t              index;
    uint64_t              count;

    const int32_t  used_space      = spa_ringbuffer_get_write_index(&audio_stream_source->ring, &index);
    const uint32_t available_space = BUFFER_SIZE - used_space;
    sf_count_t     frames_to_read  = sizeof(buffer) / stride;

    if (available_space / stride < frames_to_read) {
        frames_to_read = available_space / stride;
    }

    if (frames_to_read > 0) {
        const sf_count_t frames_read = sf_readf_short(audio_stream_source->input_snd_file_handle, buffer, frames_to_read);

        if (frames_read > 0) {
            const uint32_t bytes_to_write = frames_read * stride;
            spa_ringbuffer_write_data(&audio_stream_source->ring, audio_stream_source->buffer, BUFFER_SIZE, index % BUFFER_SIZE, buffer, bytes_to_write);
            spa_ringbuffer_write_update(&audio_stream_source->ring, index + bytes_to_write);
        } else {
            audio_stream_source->running = false;
        }
    }
}

int ctm_play_sound(const char* file) {
    CtmAudioStreamSource audio_stream_source = {0,};

    pw_init(NULL, NULL);

    audio_stream_source.input_snd_file_handle = sf_open(file, SFM_READ, &audio_stream_source.input_snd_file_info);
    if (!audio_stream_source.input_snd_file_handle) {
        fprintf(stderr, "%s: Failed to open file: %s\n", __func__, sf_strerror(NULL));
        return -1;
    }

    audio_stream_source.thread_loop           = pw_thread_loop_new("audio-src", NULL);
    audio_stream_source.loop                  = pw_thread_loop_get_loop(audio_stream_source.thread_loop);
    audio_stream_source.running               = true;

    spa_ringbuffer_init(&audio_stream_source.ring);
    if ((audio_stream_source.eventfd = spa_system_eventfd_create(audio_stream_source.loop->system, SPA_FD_CLOEXEC)) < 0) {
        fprintf(stderr, "%s: Failed to create eventfd: %d\n", __func__, errno);
        return -1;
    }

    audio_stream_source.output_playback_stream = pw_stream_new_simple(
        audio_stream_source.loop,
        "audio-src-ring",
        pw_properties_new(
            PW_KEY_MEDIA_TYPE, "Audio",
            PW_KEY_MEDIA_CATEGORY, "Playback",
            PW_KEY_MEDIA_ROLE, "Music",
            NULL),
        &stream_events,
        &audio_stream_source);

    uint8_t                buffer[1024];
    struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
    const struct spa_pod*  params[1];
    params[0] = spa_format_audio_raw_build(
        &b,
        SPA_PARAM_EnumFormat,
        &SPA_AUDIO_INFO_RAW_INIT(
            .format = SPA_AUDIO_FORMAT_S16,
            .channels = audio_stream_source.input_snd_file_info.channels,
            .rate = audio_stream_source.input_snd_file_info.samplerate,
        )
    );

    pw_thread_loop_start(audio_stream_source.thread_loop);
    pw_thread_loop_lock(audio_stream_source.thread_loop);

    pw_stream_connect(audio_stream_source.output_playback_stream,
                      PW_DIRECTION_OUTPUT,
                      PW_ID_ANY,
                      PW_STREAM_FLAG_AUTOCONNECT |
                      PW_STREAM_FLAG_MAP_BUFFERS |
                      PW_STREAM_FLAG_RT_PROCESS,
                      params, 1);

    refill_ringbuffer(&audio_stream_source);

    pw_thread_loop_unlock(audio_stream_source.thread_loop);

    // Main event loop.
    while (audio_stream_source.running) {
        uint64_t count;

        // Wait for the signal from the process callback stating that space is available.
        spa_system_eventfd_read(audio_stream_source.loop->system, audio_stream_source.eventfd, &count);

        // Lock, refill, unlock.
        pw_thread_loop_lock(audio_stream_source.thread_loop);
        refill_ringbuffer(&audio_stream_source);
        pw_thread_loop_unlock(audio_stream_source.thread_loop);
    }

    // Clean up after ourselves.
    pw_thread_loop_stop(audio_stream_source.thread_loop);

    pw_thread_loop_lock(audio_stream_source.thread_loop);
    pw_stream_destroy(audio_stream_source.output_playback_stream);
    pw_thread_loop_unlock(audio_stream_source.thread_loop);

    pw_thread_loop_destroy(audio_stream_source.thread_loop);
    close(audio_stream_source.eventfd);
    sf_close(audio_stream_source.input_snd_file_handle);
    pw_deinit();

    return 0;
}
