#include "dapper2d/ResourceLoader.hpp"

#include <random>

#include "dapper2d/AudioStream.hpp"
#include "SDL3_image/SDL_image.h"

#include "dapper2d/Renderer.hpp"

namespace Engine
{
    void ResourceLoader::Clean()
    {
        for (const auto& kvp : tracks)
        {
            MIX_DestroyTrack(kvp.first);
        }
        MIX_DestroyMixer(mixer);
        MIX_Quit();
    }

    std::shared_ptr<SDL_Texture> ResourceLoader::LoadTexture(const std::string& filePath)
    {
        if (filePath.empty())
            return nullptr;

        if (texturePathLookup.contains(filePath))
        {
            int id = texturePathLookup[filePath];
            if (id >= 0 && id < (int)textures.size())
            {
                if (auto existing = textures[id].lock())
                    return existing;
            }
        }

        SDL_Texture *rawTexture = IMG_LoadTexture(Renderer::GetRenderer(), filePath.c_str());

        if (!rawTexture)
        {
            SDL_Log("Texture failed to load: %s : %s", filePath.c_str(), SDL_GetError());
            return nullptr;
        }

        SDL_SetTextureScaleMode(rawTexture, scaleMode);
        std::shared_ptr<SDL_Texture> texture(rawTexture, TextureDeleter{});

        std::mt19937 gen(std::random_device{}());

        int textureId;
        do
        {
            std::uniform_int_distribution<> textureIdDist(0, INT32_MAX);
            textureId = textureIdDist(gen);
        } while (textures.contains(textureId));

        if (textureId >= 0)
        {
            textures[textureId] = texture;
            texturePathLookup[filePath] = textureId;
            textureIdToPath[textureId] = filePath;

            return texture;
        }

        SDL_Log("Failed to create texture id : %s : %s", filePath.c_str(), SDL_GetError());
        texture.reset();

        return nullptr;
    }

    std::shared_ptr<SDL_Texture> ResourceLoader::CreateTexture(SDL_PixelFormat format, SDL_TextureAccess access, int w, int h)
    {
        SDL_Texture* rawTexture = SDL_CreateTexture(
                Renderer::GetRenderer(),
                format,
                access,
                w,
                h
        );
        SDL_SetTextureScaleMode(rawTexture, scaleMode);

        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> textureIdDist(0, INT32_MAX);
        int textureId = textureIdDist(gen);

        std::shared_ptr<SDL_Texture> texture(rawTexture, TextureDeleter{});
        textures[textureId] = texture;
        return texture;
    }

    void ResourceLoader::CleanExpired(size_t maxPerCall)
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

    void ResourceLoader::SetScaleMode(SDL_ScaleMode _scaleMode)
    {
        scaleMode = _scaleMode;
    }

    std::shared_ptr<AudioStream> ResourceLoader::LoadSound(const std::string& filePath, SDL_PropertiesID properties)
    {
        if (filePath.empty())
            return nullptr;

        if (!mixer)
            mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

        if (tracks.size() < trackCount)
        {
            uint8_t needed = trackCount - tracks.size();
            for (uint8_t i = 0; i < needed; ++i)
            {
                tracks.emplace(MIX_CreateTrack(mixer), true);
            }
        }

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

        std::mt19937 gen(std::random_device{}());
        int soundId;
        do
        {
            soundId = std::uniform_int_distribution<>(0, INT32_MAX)(gen);
        } while (sounds.contains(soundId));

        AudioStream* audioStream = new AudioStream(stream, properties);
        std::shared_ptr<AudioStream> sound(audioStream);

        sounds[soundId] = sound;
        soundPathLookup[filePath] = soundId;
        soundIdToPath[soundId] = filePath;

        return sound;
    }

    MIX_Mixer* ResourceLoader::GetMixer()
    {
        return mixer;
    }
}
