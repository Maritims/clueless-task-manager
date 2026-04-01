//
// Created by martin on 01.04.2026.
//

#ifndef CLUELESS_TASK_MGR_CTM_SOUND_H
#define CLUELESS_TASK_MGR_CTM_SOUND_H

typedef struct CtmAudioStreamSource CtmAudioStreamSource;
typedef void (*                     CtmSoundCallback)(CtmAudioStreamSource* audio_stream_source, void* user_data);

/**
 * Boot up the sound system.
 */
void ctm_sound_init(void);

/**
 * Shut down the sound system.
 */
void ctm_sound_deinit(void);

/**
 * Play a sound asynchronously without blocking the main thread.
 */
CtmAudioStreamSource* ctm_sound_play_async(const char* file, CtmSoundCallback on_finished, void* user_data);

/**
 * Clean up after playing a sound.
 */
void ctm_sound_stop(CtmAudioStreamSource* audio_stream_source);

#endif //CLUELESS_TASK_MGR_CTM_SOUND_H
