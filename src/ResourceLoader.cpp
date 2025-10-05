#include "dapper2d/ResourceLoader.hpp"
#include "dapper2d/Renderer.hpp"

namespace Engine
{
    std::unordered_map<std::string, std::weak_ptr<SDL_Texture>> ResourceLoader::textures;
    std::unordered_map<std::string, std::weak_ptr<SDL_Texture>>::iterator ResourceLoader::cleanupIt = ResourceLoader::textures.begin();
    SDL_ScaleMode ResourceLoader::scaleMode = SDL_SCALEMODE_LINEAR;

    std::shared_ptr<SDL_Texture> ResourceLoader::LoadTexture(std::string& texturePath)
    {
        if (texturePath.empty())
            return nullptr;

        std::string fullPath = "assets/" + texturePath;

        auto existing = textures[fullPath].lock();
        if (existing)
        {
            return existing;
        }

        SDL_Texture *rawTexture = IMG_LoadTexture(Renderer::GetRenderer(), fullPath.c_str());

        if (!rawTexture)
        {
            SDL_Log("Texture failed to load: %s : %s", texturePath.c_str(), SDL_GetError());
            return nullptr;
        }

        SDL_SetTextureScaleMode(rawTexture, scaleMode);
        std::shared_ptr<SDL_Texture> texture(rawTexture, TextureDeleter{});
        textures[fullPath] = texture;

        return texture;
    }

    void ResourceLoader::CleanExpired(size_t maxPerCall)
    {
        size_t count = 0;

        while (count < maxPerCall && !textures.empty())
        {
            if (cleanupIt == textures.end())
            {
                cleanupIt = textures.begin(); // wrap around
            }

            if (cleanupIt->second.expired())
            {
                cleanupIt = textures.erase(cleanupIt);
            }
            else
            {
                ++cleanupIt;
            }

            ++count;
        }
    }

    void ResourceLoader::SetScaleMode(SDL_ScaleMode _scaleMode)
    {
        scaleMode = _scaleMode;
    }
}
