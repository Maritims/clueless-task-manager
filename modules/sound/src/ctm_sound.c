/**
 * @file ctm_sound.c
 * @author Martin Severin Steffensen
 * @date 02 April 2026
 *
 * This module provides functionality for asynchronous playback of sound files using libpipewire and libsndfile.
 * A ringbuffer and event-based architecture is used to ensure smooth playback.
 */

#include "ctm_sound.h"

#include <math.h>
#include <sndfile.h>
#include <stdio.h>
#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>
#include <spa/utils/ringbuffer.h>
#include <sys/stat.h>

#define BUFFER_SIZE (128*1024)

struct CtmAudioStreamSource {
    struct pw_thread_loop* thread_loop;            // Dedicated thread and mutex for PipeWire.
    struct pw_loop*        loop;                   // The underlying event loop instance.
    SNDFILE*               input_snd_file_handle;  // Handle for the audio file via libsndfile.
    SF_INFO                input_snd_file_info;    // Metadata such as rate, channels, etc.
    struct pw_stream*      output_playback_stream; // The PipeWire stream object for audio output.
    int                    eventfd;                // Signal mechanism to notify the main loop when the ringbuffer needs more data.
    bool                   running;                // Application-level state of the playback stream.

    CtmSoundCallback on_finished;      // The callback function to run once a sound has finished playing.
    void*            on_finished_data; // Any data we may wish to pass to the callback function.
    bool             file_eof;         // This flag indicates whether the end of the input sound file has been reached.

    // Ring buffer state.
    struct spa_ringbuffer ring;                // The ringbuffer control structure.
    int8_t                buffer[BUFFER_SIZE]; // The buffer used by the ringbuffer control structure to ensure smooth playback.
};

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

/**
 * Fill the ringbuffer with sound data.
 * @param userdata The audio stream source.
 */
static void refill_ringbuffer(void* userdata) {
    CtmAudioStreamSource* audio_stream_source = userdata;
    // The stride is the distance between audio frames.
    const int stride = sizeof(int16_t) * audio_stream_source->input_snd_file_info.channels;
    int16_t   local_buffer[4096];
    uint32_t  read_index;
    uint32_t  write_index;

    const int32_t  used_space      = spa_ringbuffer_get_write_index(&audio_stream_source->ring, &read_index); // How much space has been consumed in the ringbuffer?
    const uint32_t available_space = BUFFER_SIZE - used_space;                                                // How much room is there in the ringbuffer?
    sf_count_t     frames_to_read  = sizeof(local_buffer) / stride;                                           // How many frames can we fit in our local buffer?

    // How many frames are we actually going to read?
    if (available_space / stride < frames_to_read) {
        frames_to_read = available_space / stride;
    }

    if (frames_to_read > 0) {
        const sf_count_t frames_read = sf_readf_short(audio_stream_source->input_snd_file_handle, local_buffer, frames_to_read);

        if (frames_read > 0) {
            // We read some frames from our input file. Let's write them to the ringbuffer for playback!
            const uint32_t bytes_to_write = frames_read * stride;
            spa_ringbuffer_write_data(&audio_stream_source->ring, audio_stream_source->buffer, BUFFER_SIZE, read_index % BUFFER_SIZE, local_buffer, bytes_to_write);
            spa_ringbuffer_write_update(&audio_stream_source->ring, read_index + bytes_to_write);
        } else {
            // There aren't any more frames to read. We've reached the end of the file.
            audio_stream_source->file_eof = true;
        }
    }

    spa_ringbuffer_get_read_index(&audio_stream_source->ring, &read_index);
    spa_ringbuffer_get_write_index(&audio_stream_source->ring, &write_index);

    if (audio_stream_source->file_eof && read_index >= write_index) {
        audio_stream_source->running = false;
        if (audio_stream_source->on_finished) {
            audio_stream_source->on_finished(audio_stream_source, audio_stream_source->on_finished_data);
        }
    }
}

/**
 * Event handler. This is where the magic happens. Lock the thread, fill the ringbuffer, and unlock the thread.
 */
static void on_eventfd_signal(void* userdata, int fd, uint32_t mask) {
    CtmAudioStreamSource* audio_stream_source = userdata;
    uint64_t              count;

    spa_system_eventfd_read(audio_stream_source->loop->system, fd, &count);

    pw_thread_loop_lock(audio_stream_source->thread_loop);
    refill_ringbuffer(audio_stream_source);
    pw_thread_loop_unlock(audio_stream_source->thread_loop);
}

/**
 * Guard against multiple initialisations of the sound system.
 */
static bool is_initialized = false;

void ctm_sound_init(void) {
    if (is_initialized) {
        return;
    }
    pw_init(NULL, NULL);
    is_initialized = true;
}

void ctm_sound_deinit(void) {
    if (!is_initialized) {
        return;
    }
    pw_deinit();
    is_initialized = false;
}

CtmAudioStreamSource* ctm_sound_play_async(const char* file, CtmSoundCallback on_finished, void* user_data) {
    // Boot up the sound system.
    ctm_sound_init();

    // Create the audio stream source which we'll use to play the sound.
    CtmAudioStreamSource* audio_stream_source = malloc(sizeof(CtmAudioStreamSource));
    if (audio_stream_source == NULL) {
        fprintf(stderr, "%s: Failed to allocate memory\n", __func__);
        return NULL;
    }

    // Start with a blank slate.
    memset(audio_stream_source, 0, sizeof(CtmAudioStreamSource));

    // State that we haven't reached the end of the input file yet (that would have been weird!).
    audio_stream_source->file_eof = false;

    // Wire the callback function.
    audio_stream_source->on_finished      = on_finished;
    audio_stream_source->on_finished_data = user_data;

    // Open the sound file and store the file handle.
    audio_stream_source->input_snd_file_handle = sf_open(file, SFM_READ, &audio_stream_source->input_snd_file_info);
    if (!audio_stream_source->input_snd_file_handle) {
        fprintf(stderr, "%s: Failed to open file %s: %s\n", __func__, file, sf_strerror(NULL));
        free(audio_stream_source);
        return NULL;
    }

    // Create the loop and signal that the audio stream source is up and running.
    audio_stream_source->thread_loop = pw_thread_loop_new("audio-src", NULL);
    audio_stream_source->loop        = pw_thread_loop_get_loop(audio_stream_source->thread_loop);
    audio_stream_source->running     = true;

    // Initialise the ringbuffer to use for smooth playback.
    spa_ringbuffer_init(&audio_stream_source->ring);

    // Create and wire the file descriptor used for driving the event based architecture so we know when to do what.
    audio_stream_source->eventfd = spa_system_eventfd_create(audio_stream_source->loop->system, SPA_FD_CLOEXEC);
    pw_loop_add_io(audio_stream_source->loop, audio_stream_source->eventfd, SPA_IO_IN, true, on_eventfd_signal, audio_stream_source);

    // Establish the playback stream which we'll transmit the data from our input file to.
    audio_stream_source->output_playback_stream = pw_stream_new_simple(
        audio_stream_source->loop,
        "audio-src-ring",
        pw_properties_new(
            PW_KEY_MEDIA_TYPE, "Audio",
            PW_KEY_MEDIA_CATEGORY, "Playback",
            PW_KEY_MEDIA_ROLE, "Music",
            NULL),
        &stream_events,
        audio_stream_source);

    uint8_t                buffer[1024];
    struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
    const struct spa_pod*  params[1];
    params[0] = spa_format_audio_raw_build(
        &b,
        SPA_PARAM_EnumFormat,
        &SPA_AUDIO_INFO_RAW_INIT(
            .format = SPA_AUDIO_FORMAT_S16,
            .channels = audio_stream_source->input_snd_file_info.channels,
            .rate = audio_stream_source->input_snd_file_info.samplerate,
        )
    );

    // Start the thread, lock it, connect the stream, fill the ringbuffer, unlock and let the event-based architecture take us towards and across the finish line in the background.
    pw_thread_loop_start(audio_stream_source->thread_loop);
    pw_thread_loop_lock(audio_stream_source->thread_loop);

    pw_stream_connect(audio_stream_source->output_playback_stream,
                      PW_DIRECTION_OUTPUT,
                      PW_ID_ANY,
                      PW_STREAM_FLAG_AUTOCONNECT |
                      PW_STREAM_FLAG_MAP_BUFFERS |
                      PW_STREAM_FLAG_RT_PROCESS,
                      params, 1);

    refill_ringbuffer(audio_stream_source);

    pw_thread_loop_unlock(audio_stream_source->thread_loop);
    return audio_stream_source;
}

void ctm_sound_stop(CtmAudioStreamSource* audio_stream_source) {
    if (audio_stream_source) {
        // Stop the thread before doing anything else.
        pw_thread_loop_stop(audio_stream_source->thread_loop);

        // Lock the thread, destroy the stream and unlock the thread.
        pw_thread_loop_lock(audio_stream_source->thread_loop);
        pw_stream_destroy(audio_stream_source->output_playback_stream);
        pw_thread_loop_unlock(audio_stream_source->thread_loop);

        // Destroy the thread, destroy file descriptors and handles, then free the memory.
        pw_thread_loop_destroy(audio_stream_source->thread_loop);
        close(audio_stream_source->eventfd);
        sf_close(audio_stream_source->input_snd_file_handle);
        free(audio_stream_source);
    }
}
