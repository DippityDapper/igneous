#include "dapper2d/Sprite.hpp"

#include "SDL3/SDL.h"

#include "dapper2d/ResourceLoader.hpp"

namespace Engine
{
    Sprite::Sprite(const Sprite& sprite)
    {
        texture = sprite.texture;
        atlasW = sprite.atlasW;
        atlasH = sprite.atlasH;
        atlasX = sprite.atlasX;
        atlasY = sprite.atlasY;
    }

    Sprite::Sprite(const std::string& filePath)
    {
        texture = ResourceLoader::LoadTexture(filePath);
        if (texture)
        {
            SDL_GetTextureSize(texture.get(), &w, &h);
            atlasW = w;
            atlasH = h;
        }
    }

    Sprite::Sprite(const std::string& filePath, float _w, float _h, int _x, int _y)
    {
        texture = ResourceLoader::LoadTexture(filePath);
        if (texture)
        {
            SDL_GetTextureSize(texture.get(), &w, &h);
            atlasW = _w;
            atlasH = _h;
        }

        atlasX = _x;
        atlasY = _y;
    }

    SDL_Texture *Sprite::GetTexture()
    {
        return texture.get();
    }

    SDL_FRect Sprite::GetSourceRect() const
    {
        SDL_FRect src;

        src.w = atlasW;
        src.h = atlasH;

        src.x = ((float)atlasX * atlasW);
        src.y = ((float)atlasY * atlasH);

        return src;
    }
}