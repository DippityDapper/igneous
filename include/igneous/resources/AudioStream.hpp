// Doc: docs/classes/AudioStream.md
#pragma once

#include "SDL3_mixer/SDL_mixer.h"

namespace Engine
{

    /**
     * @brief Wrapper around SDL_mixer MIX_Audio with playback properties.
     */
    class AudioStream
    {
      public:

        MIX_Audio* stream = nullptr;

        SDL_PropertiesID properties = 0;

      public:

        AudioStream(MIX_Audio* _stream, SDL_PropertiesID _properties);

        ~AudioStream();

        void Play(float gain);

        void SetProperties(SDL_PropertiesID _properties);
    };
}
