#include "dapper2d/ResourceLoader.hpp"

#include <random>

#include "SDL3_image/SDL_image.h"

#include "dapper2d/Renderer.hpp"

namespace Engine
{
    std::unordered_map<int, std::weak_ptr<SDL_Texture>> ResourceLoader::textures{};
    std::unordered_map<std::string, int> ResourceLoader::textureMap{};
    std::unordered_map<int, std::string> ResourceLoader::idToPath;
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

        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> textureIdDist(0, INT32_MAX);
        int textureId = textureIdDist(gen);

        textures[textureId] = texture;
        textureMap[fullPath] = textureId;
        idToPath[textureId] = fullPath;

        return texture;
    }

    void ResourceLoader::CleanExpired(size_t maxPerCall)
    {
        if (textures.empty())
            return;

        size_t cleaned = 0;

        for (auto it = textures.begin(); it != textures.end() && cleaned < maxPerCall; )
        {
            if (it->second.expired())
            {
                textureMap.erase(idToPath[it->first]);
                idToPath.erase(it->first);
                it = textures.erase(it);
                ++cleaned;
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
}
