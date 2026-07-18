#include "igneous/resources/ResourceManager.hpp"

#include <ranges>

#include "igneous/resources/AudioStream.hpp"
#include "SDL3_image/SDL_image.h"

#include "igneous/rendering/Renderer.hpp"

namespace Engine
{
    void ResourceManager::ResetForTests()
    {
        spritesByZIndex.clear();
        spriteIterators.clear();
        textures.clear();
        texturePathLookup.clear();
        textureIdToPath.clear();
        scaleMode = SDL_SCALEMODE_LINEAR;
        sounds.clear();
        soundPathLookup.clear();
        soundIdToPath.clear();
        nextSpriteId = 1;
        nextTextureId = 1;
        nextSoundId = 1;

        if (mixer)
        {
            for (const auto& kvp: tracks)
            {
                MIX_DestroyTrack(kvp.first);
            }
            tracks.clear();
            MIX_DestroyMixer(mixer);
            mixer = nullptr;
            MIX_Quit();
        }
    }

    int ResourceManager::AllocateSpriteId()
    {
        return nextSpriteId++;
    }

    int ResourceManager::AllocateTextureId()
    {
        return nextTextureId++;
    }

    int ResourceManager::AllocateSoundId()
    {
        return nextSoundId++;
    }

    void ResourceManager::Clean()
    {
        spritesByZIndex.clear();
        spriteIterators.clear();

        for (const auto& kvp: tracks)
        {
            MIX_DestroyTrack(kvp.first);
        }
        MIX_DestroyMixer(mixer);
        MIX_Quit();
    }

    bool ResourceManager::RegisterSprite(Sprite* sprite)
    {
        if (spriteIterators.contains(sprite->id))
            return false;

        sprite->id = AllocateSpriteId();

        auto it = spritesByZIndex.emplace(sprite->GetZIndex(), sprite);
        spriteIterators[sprite->id] = it;

        return true;
    }

    bool ResourceManager::UnregisterSprite(int spriteId)
    {
        if (spriteIterators.empty())
            return false;

        auto it = spriteIterators.find(spriteId);
        if (it == spriteIterators.end())
            return false;

        spritesByZIndex.erase(it->second);
        spriteIterators.erase(it);

        return true;
    }

    bool ResourceManager::UpdateSpriteZIndex(Sprite* sprite, int newZIndex)
    {
        auto it = spriteIterators.find(sprite->id);
        if (it == spriteIterators.end())
            return false;

        spritesByZIndex.erase(it->second);

        auto newIt = spritesByZIndex.emplace(newZIndex, sprite);
        spriteIterators[sprite->id] = newIt;

        return true;
    }

    std::shared_ptr<SDL_Texture> ResourceManager::LoadTexture(const std::string& filePath)
    {
        if (filePath.empty())
            return nullptr;

        if (texturePathLookup.contains(filePath))
        {
            int id = texturePathLookup[filePath];
            if (textures.contains(id))
            {
                if (auto existing = textures[id].lock())
                    return existing;
            }
        }

        SDL_Texture* rawTexture = IMG_LoadTexture(Renderer::GetRenderer(), filePath.c_str());

        if (!rawTexture)
        {
            SDL_Log("Texture failed to load: %s : %s", filePath.c_str(), SDL_GetError());
            return nullptr;
        }

        SDL_SetTextureScaleMode(rawTexture, scaleMode);
        std::shared_ptr<SDL_Texture> texture(rawTexture, TextureDeleter{});

        const int textureId = AllocateTextureId();
        textures[textureId] = texture;
        texturePathLookup[filePath] = textureId;
        textureIdToPath[textureId] = filePath;

        return texture;
    }

    std::shared_ptr<SDL_Texture> ResourceManager::CreateTexture(SDL_PixelFormat format, SDL_TextureAccess access, int w, int h)
    {
        SDL_Texture* rawTexture = SDL_CreateTexture(
                Renderer::GetRenderer(),
                format,
                access,
                w,
                h);
        SDL_SetTextureScaleMode(rawTexture, scaleMode);

        const int textureId = AllocateTextureId();

        std::shared_ptr<SDL_Texture> texture(rawTexture, TextureDeleter{});
        textures[textureId] = texture;
        return texture;
    }

    void ResourceManager::CleanExpired(size_t maxPerCall)
    {
        if (textures.empty())
            return;

        size_t texturesCleaned = 0;

        for (auto it = textures.begin(); it != textures.end() && texturesCleaned < maxPerCall;)
        {
            if (it->second.expired())
            {
                int id = it->first;
                texturePathLookup.erase(textureIdToPath[id]);
                textureIdToPath.erase(id);
                it = textures.erase(it);
                ++texturesCleaned;
            }
            else
            {
                ++it;
            }
        }

        size_t audioCleaned = 0;

        for (auto it = sounds.begin(); it != sounds.end() && audioCleaned < maxPerCall;)
        {
            if (it->second.expired())
            {
                int id = it->first;
                soundPathLookup.erase(soundIdToPath[id]);
                soundIdToPath.erase(id);
                it = sounds.erase(it);
                ++audioCleaned;
            }
            else
            {
                ++it;
            }
        }
    }

    void ResourceManager::RenderSprites(double delta)
    {
        for (const auto& it: spritesByZIndex)
        {
            if (it.second)
            {
                Sprite* sprite = it.second;
                if (sprite->render)
                    Renderer::BufferAdd(sprite->position, sprite, nullptr);

                Animation* animation = sprite->GetCurrentAnimation();
                if (animation)
                    animation->IncrementElapsedTime(delta);
            }
        }
    }

    void ResourceManager::SetScaleMode(SDL_ScaleMode _scaleMode)
    {
        scaleMode = _scaleMode;
    }

    bool ResourceManager::EnsureAudioTracks()
    {
        if (!MIX_Init())
        {
            SDL_Log("MIX init failed: %s", SDL_GetError());
            return false;
        }

        if (!mixer)
            mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

        if (!mixer)
        {
            SDL_Log("Failed to create audio mixer: %s", SDL_GetError());
            return false;
        }

        if (tracks.size() < trackCount)
        {
            const uint8_t needed = trackCount - static_cast<uint8_t>(tracks.size());
            for (uint8_t i = 0; i < needed; ++i)
            {
                MIX_Track* track = MIX_CreateTrack(mixer);
                if (!track)
                {
                    SDL_Log("Failed to create audio track: %s", SDL_GetError());
                    return false;
                }
                tracks.emplace(track, true);
            }
        }

        return true;
    }

    MIX_Track* ResourceManager::AcquireAudioTrack()
    {
        if (!EnsureAudioTracks())
            return nullptr;

        MIX_Track* available = nullptr;
        for (const auto& kvp: tracks)
        {
            if (kvp.second)
            {
                available = kvp.first;
                tracks[available] = false;
                break;
            }
        }

        if (!available)
        {
            if (tracks.empty())
            {
                SDL_Log("No audio tracks available");
                return nullptr;
            }

            MIX_Track* track = tracks.begin()->first;
            MIX_StopTrack(track, 0);
            available = track;
            SDL_Log("Too many sounds playing at once!");
        }

        return available;
    }

    std::shared_ptr<AudioStream> ResourceManager::LoadSound(const std::string& filePath, SDL_PropertiesID properties)
    {
        if (filePath.empty())
            return nullptr;

        if (!EnsureAudioTracks())
            return nullptr;

        if (soundPathLookup.contains(filePath))
        {
            int id = soundPathLookup[filePath];
            if (auto existing = sounds[id].lock())
                return existing;
        }

        MIX_Audio* stream = MIX_LoadAudio(nullptr, filePath.c_str(), true);
        if (!stream)
        {
            SDL_Log("Failed to load audio stream: %s : %s", filePath.c_str(), SDL_GetError());
            return nullptr;
        }

        const int soundId = AllocateSoundId();

        AudioStream* audioStream = new AudioStream(stream, properties);
        std::shared_ptr<AudioStream> sound(audioStream);

        sounds[soundId] = sound;
        soundPathLookup[filePath] = soundId;
        soundIdToPath[soundId] = filePath;

        return sound;
    }

    MIX_Mixer* ResourceManager::GetMixer()
    {
        return mixer;
    }
}
