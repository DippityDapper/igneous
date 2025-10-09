#include "dapper2d/ResourceLoader.hpp"

#include "SDL3_image/SDL_image.h"

#include "dapper2d/Renderer.hpp"

namespace Engine
{
    std::vector<std::weak_ptr<SDL_Texture>> ResourceLoader::textures{};
    std::unordered_map<std::string, int> ResourceLoader::textureMap{};
    size_t ResourceLoader::cleanupIndex = 0;
    SDL_ScaleMode ResourceLoader::scaleMode = SDL_SCALEMODE_LINEAR;

    std::shared_ptr<SDL_Texture> ResourceLoader::LoadTexture(const std::string& texturePath)
    {
        if (texturePath.empty())
            return nullptr;

        std::string fullPath = "assets/" + texturePath;

        if (textureMap.contains(fullPath))
        {
            int index = textureMap[fullPath];
            if (index >= 0 && index < (int)textures.size())
            {
                if (auto existing = textures[index].lock())
                    return existing;
            }
        }

        SDL_Texture *rawTexture = IMG_LoadTexture(Renderer::GetRenderer(), fullPath.c_str());

        if (!rawTexture)
        {
            SDL_Log("Texture failed to load: %s : %s", texturePath.c_str(), SDL_GetError());
            return nullptr;
        }

        SDL_SetTextureScaleMode(rawTexture, scaleMode);
        std::shared_ptr<SDL_Texture> texture(rawTexture, TextureDeleter{});

        int textureIndex = textures.size();
        textures.push_back(texture);
        textureMap[fullPath] = textureIndex;

        return texture;
    }

    void ResourceLoader::CleanExpired(size_t maxPerCall)
    {
        if (textures.empty())
            return;

        size_t count = 0;

        while (count < maxPerCall && !textures.empty())
        {
            if (cleanupIndex >= textures.size())
                cleanupIndex = 0;

            if (textures[cleanupIndex].expired())
            {
                for (auto it = textureMap.begin(); it != textureMap.end();)
                {
                    if (it->second == cleanupIndex)
                        it = textureMap.erase(it);
                    else
                        ++it;
                }

                textures[cleanupIndex].reset();
            }

            ++cleanupIndex;
            ++count;
        }
    }

    void ResourceLoader::SetScaleMode(SDL_ScaleMode _scaleMode)
    {
        scaleMode = _scaleMode;
    }

    std::shared_ptr<SDL_Texture>
    ResourceLoader::CreateTexture(SDL_PixelFormat format, SDL_TextureAccess access, int w, int h)
    {
        SDL_Texture* rawTexture = SDL_CreateTexture(
                Renderer::GetRenderer(),
                format,
                access,
                w,
                h
        );
        SDL_SetTextureScaleMode(rawTexture, scaleMode);

        std::shared_ptr<SDL_Texture> texture(rawTexture, TextureDeleter{});
        textures.push_back(texture);
        return texture;
    }
}
