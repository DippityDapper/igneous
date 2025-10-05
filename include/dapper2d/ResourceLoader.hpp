#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"

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
        static std::unordered_map<std::string, std::weak_ptr<SDL_Texture>> textures;
        static std::unordered_map<std::string, std::weak_ptr<SDL_Texture>>::iterator cleanupIt;
        static SDL_ScaleMode scaleMode;

    public:
        static std::shared_ptr<SDL_Texture> LoadTexture(std::string& texturePath);
        static void CleanExpired(size_t maxPerCall = 10);
        static void SetScaleMode(SDL_ScaleMode _scaleMode);
    };
}
