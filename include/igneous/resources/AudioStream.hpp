#pragma once

#include "SDL3_mixer/SDL_mixer.h"

namespace Engine
{
    /**
     * @class AudioStream
     * @brief Wraps an SDL_mixer audio stream for playback control.
     *
     * The AudioStream class manages a single audio file loaded by SDL_mixer.
     * It handles playback on available audio tracks with gain control and
     * custom properties.
     *
     * ## Track Management
     *
     * AudioStreams don't own tracks; instead, they use available tracks from
     * the ResourceManager's track pool. When playing, the stream finds an
     * available track, or stops the oldest playing track if none are free.
     *
     * ## Properties
     *
     * SDL_PropertiesID allows passing custom properties to the mixer for
     * advanced audio control (looping, fade-in, etc.).
     *
     * @note AudioStreams are created by ResourceManager::LoadSound().
     * @note Destruction automatically releases the underlying MIX_Audio.
     * @note Uses the ResourceManager's track pool for playback.
     *
     * @see ResourceManager
     */
    class AudioStream
    {
      public:
        /**
         * @brief Pointer to the SDL_mixer audio data.
         *
         * Loaded by MIX_LoadAudio() and destroyed by MIX_DestroyAudio()
         * in the destructor.
         *
         * @note Do not manually destroy; managed by the AudioStream.
         */
        MIX_Audio* stream = nullptr;

        /**
         * @brief SDL properties ID for playback customization.
         *
         * Used to pass custom properties to SDL_mixer during playback,
         * such as loop count, fade durations, etc.
         *
         * @note Set during construction or via SetProperties().
         */
        SDL_PropertiesID properties = 0;

      public:
        /**
         * @brief Constructs an AudioStream with audio data and properties.
         *
         * @param _stream Pointer to the MIX_Audio loaded by SDL_mixer.
         * @param _properties SDL properties ID for playback customization.
         *
         * @note Typically called by ResourceManager::LoadSound().
         */
        AudioStream(MIX_Audio* _stream, SDL_PropertiesID _properties);

        /**
         * @brief Destructor that releases the audio data.
         *
         * Calls MIX_DestroyAudio() to free the audio stream.
         */
        ~AudioStream();

        /**
         * @brief Plays the audio stream on an available track.
         *
         * Finds an available track from the ResourceManager's track pool and
         * plays the audio with the specified gain. If no tracks are available,
         * stops the oldest playing track and uses it.
         *
         * A callback is registered to mark the track as available when playback
         * finishes.
         *
         * @param gain Volume multiplier for playback (0.0 = silence, 1.0 = normal).
         *             Values are clamped to a maximum of 10.0.
         *
         * @note Logs a warning if all tracks are in use (forces oldest to stop).
         * @note The track is automatically freed when playback finishes.
         */
        void Play(float gain);

        /**
         * @brief Updates the playback properties.
         *
         * Changes the SDL properties used for future playback of this audio.
         *
         * @param _properties The new SDL properties ID.
         *
         * @note Does not affect currently playing instances.
         */
        void SetProperties(SDL_PropertiesID _properties);
    };
}