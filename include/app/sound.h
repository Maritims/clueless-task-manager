/**
 * @file sound.h
 * @author Martin Severin Steffensen
 * @date 02 April 2026
 * @brief Sound system API.
 */

#ifndef CTM_SOUND_H
#define CTM_SOUND_H

typedef struct CtmAudioStreamSource CtmAudioStreamSource;

typedef void (*CtmSoundCallback)(CtmAudioStreamSource* audio_stream_source,
                                 void*                 user_data);

/**
 * @brief Boots up the sound system.
 * * @note This function must be executed before the sound system can be used.
 */
void ctm_sound_init(void);

/**
 * @brief Shuts down the sound system.
 * * @note This function must be executed once the sound system is no longer used.
 */
void ctm_sound_deinit(void);

/**
 * @brief Plays a sound asynchronously without blocking the main thread.
 * * @param file The path to the sound file.
 * * @param on_finished The function to execute once a sound is done playing. This is typically some function which in turn calls ctm_sound_stop.
 * * @param user_data Any argument you wish to pass to the callback function.
 * * @return The audio stream source in use, or NULL if the sound cannot be played for any reason.
 */
CtmAudioStreamSource* ctm_sound_play_async(const char*      file,
                                           CtmSoundCallback on_finished,
                                           void*            user_data);

/**
 * @brief Cleans up after playing a sound.
 * * @note This function must always be executed once a sound has finished playing.
 * * @param audio_stream_source The audio stream source in use.
 */
void ctm_sound_stop(CtmAudioStreamSource* audio_stream_source);

#endif
