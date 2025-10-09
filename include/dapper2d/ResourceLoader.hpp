#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "SDL3/SDL.h"

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

    class ResourceLoader
    {
    private:
        static std::vector<std::weak_ptr<SDL_Texture>> textures;
        static std::unordered_map<std::string, int> textureMap;
        static size_t cleanupIndex;
        static SDL_ScaleMode scaleMode;

    public:
        static std::shared_ptr<SDL_Texture> LoadTexture(const std::string& texturePath);
        static std::shared_ptr<SDL_Texture> CreateTexture(SDL_PixelFormat format, SDL_TextureAccess access, int w, int h);
        static void CleanExpired(size_t maxPerCall = 10);
        static void SetScaleMode(SDL_ScaleMode _scaleMode);
    };
}
