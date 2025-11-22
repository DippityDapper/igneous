#include "dapper2d/AudioStream.hpp"

#include "dapper2d/ResourceLoader.hpp"

namespace Engine
{
    AudioStream::AudioStream(MIX_Audio* _stream, SDL_PropertiesID _properties)
    {
        stream = _stream;
        properties = _properties;
    }

    AudioStream::~AudioStream()
    {
        MIX_DestroyAudio(stream);
    }

    void AudioStream::Play()
    {
        MIX_Track* available = nullptr;
        for (const auto& kvp : ResourceLoader::tracks)
        {
            if (kvp.second)
            {
                available = kvp.first;
                ResourceLoader::tracks[available] = false;
                break;
            }
        }

        if (!available)
        {
            MIX_Track* track = ResourceLoader::tracks.begin()->first;
            MIX_StopTrack(track, 0);
            available = track;
            SDL_Log("Too many sounds playing at once!");
        }

        MIX_SetTrackStoppedCallback
        (
            available,
            [](void*, MIX_Track* track)
                {
                    ResourceLoader::tracks[track] = true;
                },
        nullptr
        );

        MIX_SetTrackAudio(available, stream);
        MIX_PlayTrack(available, properties);
    }

    void AudioStream::SetProperties(SDL_PropertiesID _properties)
    {
        properties = _properties;
    }
}
