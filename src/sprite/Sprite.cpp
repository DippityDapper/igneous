#include "engine/entity/Sprite.h"
#include "engine/resources/ResourceLoader.h"

namespace Engine
{
    Sprite::Sprite(std::string& texturePath)
    {
        texture = ResourceLoader::LoadTexture(texturePath);
        if (texture)
        {
            SDL_GetTextureSize(texture, &w, &h);
            tileW = w;
            tileH = h;
        }
    }

    Sprite::Sprite(std::string& texturePath, float _w, float _h, int _x, int _y)
    {
        texture = ResourceLoader::LoadTexture(texturePath);
        if (texture)
        {
            SDL_GetTextureSize(texture, &w, &h);
            tileW = _w;
            tileH = _h;
        }

        x = _x;
        y = _y;
    }

    SDL_Texture *Sprite::GetTexture()
    {
        return texture;
    }

    SDL_FRect Sprite::GetSourceRect()
    {
        SDL_FRect src;

        src.w = tileW;
        src.h = tileH;

        int tilesPerRow = w / tileW;
        int tilesPerCol = h / tileH;

        int wrappedX = ((x % tilesPerRow) + tilesPerRow) % tilesPerRow;
        int wrappedY = ((y % tilesPerCol) + tilesPerCol) % tilesPerCol;

        src.x = (float)(wrappedX * tileW);
        src.y = (float)(wrappedY * tileH);

        return src;
    }
}