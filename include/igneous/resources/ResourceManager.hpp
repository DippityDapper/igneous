#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <memory>

#include "igneous/resources/Sprite.hpp"
#include "SDL3_mixer/SDL_mixer.h"

namespace Engine
{

    class AudioStream;
}

namespace Engine
{

    struct TextureDeleter
    {

        void operator()(SDL_Texture* texture) const noexcept
        {
            if (texture)
            {
                SDL_DestroyTexture(texture);
            }
        }
    };

    class ResourceManager
    {
      private:

        static inline std::multimap<int, Sprite*> spritesByZIndex{};

        static inline std::unordered_map<int, std::multimap<int, Sprite*>::iterator> spriteIterators{};

        static inline std::unordered_map<int, std::weak_ptr<SDL_Texture>> textures{};

        static inline std::unordered_map<std::string, int> texturePathLookup{};

        static inline std::unordered_map<int, std::string> textureIdToPath{};

        static inline SDL_ScaleMode scaleMode = SDL_SCALEMODE_LINEAR;

        static inline std::unordered_map<int, std::weak_ptr<AudioStream>> sounds{};

        static inline std::unordered_map<std::string, int> soundPathLookup{};

        static inline std::unordered_map<int, std::string> soundIdToPath{};

      public:

        static inline MIX_Mixer* mixer = nullptr;

        static inline uint8_t trackCount = 16;

        static inline std::unordered_map<MIX_Track*, bool> tracks{};

      public:

        static void Clean();

        /// Clears static resource state between unit tests.
        static void ResetForTests();

      public:

        ResourceManager(const ResourceManager& other) = delete;

        static bool RegisterSprite(Sprite* sprite);

        static bool UnregisterSprite(int spriteId);

        static bool UpdateSpriteZIndex(Sprite* sprite, int newZIndex);

        static std::shared_ptr<SDL_Texture> LoadTexture(const std::string& filePath);

        static std::shared_ptr<SDL_Texture> CreateTexture(SDL_PixelFormat format, SDL_TextureAccess access, int w, int h);

        static void CleanExpired(size_t maxPerCall = 10);

        static void RenderSprites(double delta);

        static void SetScaleMode(SDL_ScaleMode _scaleMode);

        static std::shared_ptr<AudioStream> LoadSound(const std::string& filePath, SDL_PropertiesID properties);

        static MIX_Mixer* GetMixer();
    };
}
