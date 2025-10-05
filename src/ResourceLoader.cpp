#include "dapper2d/ResourceLoader.hpp"
#include "dapper2d/Renderer.hpp"

namespace Engine
{
    std::unordered_map<std::string, SDL_Texture*> ResourceLoader::textures;

    SDL_Texture* ResourceLoader::LoadTexture(std::string& texturePath)
    {
        if (texturePath.empty())
            return nullptr;

        std::string fullPath = "assets/" + texturePath;

        if (textures.contains(fullPath))
        {
            return textures[fullPath];
        }

        SDL_Texture *texture = IMG_LoadTexture(Renderer::GetRenderer(), fullPath.c_str());

        if (!texture)
        {
            SDL_Log("Texture failed to load: %s : %s", texturePath.c_str(), SDL_GetError());
            return nullptr;
        }

        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
        textures[fullPath] = texture;

        return texture;
    }

    void ResourceLoader::UnloadTexture(std::string& texturePath)
    {
        std::string fullPath = "assets/" + texturePath;
        if (textures.contains(fullPath))
        {
            SDL_DestroyTexture(textures[fullPath]);
            textures.erase(fullPath);
        }
    }

    void ResourceLoader::UnloadAll()
    {
        for (auto& pair : textures)
        {
            SDL_DestroyTexture(pair.second);
        }
        textures.clear();
    }
}
