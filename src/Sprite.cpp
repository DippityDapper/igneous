#include "dapper2d/Sprite.hpp"

#include "SDL3/SDL.h"

#include "dapper2d/ResourceLoader.hpp"

namespace Engine
{
    Sprite::Sprite(const std::string& texturePath)
    {
        texture = ResourceLoader::LoadTexture(texturePath);
        if (texture)
        {
            SDL_GetTextureSize(texture.get(), &w, &h);
            tileW = w;
            tileH = h;
        }
    }

    Sprite::Sprite(Sprite &sprite)
    {
        texture = sprite.texture;
        tileW = sprite.tileW;
        tileH = sprite.tileH;
        x = sprite.x;
        y = sprite.y;
    }

    Sprite::Sprite(const std::string& texturePath, float _w, float _h, int _x, int _y)
    {
        texture = ResourceLoader::LoadTexture(texturePath);
        if (texture)
        {
            SDL_GetTextureSize(texture.get(), &w, &h);
            tileW = _w;
            tileH = _h;
        }

        x = _x;
        y = _y;
    }

    SDL_Texture *Sprite::GetTexture()
    {
        return texture.get();
    }

    SDL_FRect Sprite::GetSourceRect() const
    {
        SDL_FRect src;

        src.w = tileW;
        src.h = tileH;

        src.x = ((float)x * tileW);
        src.y = ((float)y * tileH);

        return src;
    }
}