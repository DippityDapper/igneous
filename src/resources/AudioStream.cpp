#include "igneous/resources/AudioStream.hpp"

#include "igneous/resources/ResourceManager.hpp"

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

    void AudioStream::Play(float gain)
    {
        MIX_Track* available = nullptr;
        for (const auto& kvp : ResourceManager::tracks)
        {
            if (kvp.second)
            {
                available = kvp.first;
                ResourceManager::tracks[available] = false;
                break;
            }
        }

        if (!available)
        {
            MIX_Track* track = ResourceManager::tracks.begin()->first;
            MIX_StopTrack(track, 0);
            available = track;
            SDL_Log("Too many sounds playing at once!");
        }

        MIX_SetTrackStoppedCallback
        (
            available,
            [](void*, MIX_Track* track)
                {
                    ResourceManager::tracks[track] = true;
                },
        nullptr
        );

        if (gain > 10.0f)
            gain = 10.0f;

        MIX_SetTrackAudio(available, stream);
        MIX_SetTrackGain(available, gain);
        MIX_PlayTrack(available, properties);
    }

    void AudioStream::SetProperties(SDL_PropertiesID _properties)
    {
        properties = _properties;
    }
}
