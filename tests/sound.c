#define _DEFAULT_SOURCE
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "sound.h"

static void sound_finished_handler(CtmAudioStreamSource* audio_stream_source, void* user_data) {
    int* done = user_data;
    (void)audio_stream_source;
    *done = true;
    printf("Sound finished! Callback triggered.\n");
}

void test_play_sound(void) {
    int is_done = false;

    CtmAudioStreamSource* audio_stream_source = ctm_sound_play_async("/home/martin/Nedlastinger/Wilhelm Scream Remastered.wav", sound_finished_handler, &is_done);
    assert(audio_stream_source != NULL);

    while (!is_done) {
        usleep(10000);
    }

    ctm_sound_stop(audio_stream_source);
    ctm_sound_deinit();
    printf("%s passed\n", "test_play_sound");
}

int main(void) {
    test_play_sound();
    printf("All tests passed\n");
    return 0;
}
