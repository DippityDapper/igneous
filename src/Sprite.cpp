#include <utility>

#include "igneous/Sprite.hpp"

#include "igneous/Camera.hpp"
#include "SDL3/SDL.h"

#include "igneous/ResourceLoader.hpp"

namespace Engine
{
    Sprite::Sprite(const Sprite& sprite) : position(sprite.position)
    {
        texture = sprite.texture;
        atlasW = sprite.atlasW;
        atlasH = sprite.atlasH;
        atlasX = sprite.atlasX;
        atlasY = sprite.atlasY;
        ResourceLoader::RegisterSprite(this);
    }

    Sprite::Sprite(Vec2<float>& pos, const std::string& filePath) : position(pos)
    {
        texture = ResourceLoader::LoadTexture(filePath);
        if (texture)
        {
            SDL_GetTextureSize(texture.get(), &w, &h);
            atlasW = w;
            atlasH = h;
        }
        ResourceLoader::RegisterSprite(this);
    }

    Sprite::Sprite(Vec2<float>& pos, std::shared_ptr<SDL_Texture> text) : position(pos)
    {
        texture = std::move(text);
        if (texture)
        {
            SDL_GetTextureSize(texture.get(), &w, &h);
            atlasW = w;
            atlasH = h;
        }
        ResourceLoader::RegisterSprite(this);
    }

    Sprite::Sprite(Vec2<float>& pos, const std::string& filePath, float _w, float _h, int _x, int _y) : position(pos)
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
        ResourceLoader::RegisterSprite(this);
    }

    Sprite::Sprite(Vec2<float>& pos, std::shared_ptr<SDL_Texture> text, float _w, float _h, int _x, int _y) : position(pos)
    {
        texture = std::move(text);
        if (texture)
        {
            SDL_GetTextureSize(texture.get(), &w, &h);
            atlasW = _w;
            atlasH = _h;
        }

        atlasX = _x;
        atlasY = _y;
        ResourceLoader::RegisterSprite(this);
    }

    Sprite::~Sprite()
    {
        ResourceLoader::UnregisterSprite(id);
    }

    bool Sprite::SetTexture(const std::string& filePath)
    {
        std::shared_ptr<SDL_Texture> newTexture = ResourceLoader::LoadTexture(filePath);
        if (!newTexture)
            return false;

        texture = newTexture;
        return true;
    }

    SDL_Texture *Sprite::GetTexture()
    {
        return texture.get();
    }

    void Sprite::SetAtlas(float w, float h, int x, int y)
    {
        atlasW = w;
        atlasH = h;
        atlasX = x;
        atlasY = y;
    }

    void Sprite::SetZIndex(int z)
    {
        if (zIndex == z)
            return;

        zIndex = z;
        ResourceLoader::UpdateSpriteZIndex(this, z);
    }

    int Sprite::GetZIndex()
    {
        return zIndex;
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

    bool Sprite::IsMouseWithin(Camera* camera)
    {
        if (camera == nullptr)
            camera = Camera::main;

        float posX = position.x;
        float posY = position.y;

        float spriteW = atlasW * scaleX;
        float spriteH = atlasH * scaleY;

        float widthOffset = spriteW / 2.0f;
        float heightOffset = spriteH / 2.0f;

        float boundsLowerX = posX - widthOffset;
        float boundsLowerY = posY - heightOffset;
        float boundsUpperX = posX + widthOffset;
        float boundsUpperY = posY + heightOffset;

        Vec2<float> mousePosition = camera->GetMouseGlobalPosition();

        if ((mousePosition.x > boundsLowerX && mousePosition.x < boundsUpperX) && (mousePosition.y > boundsLowerY && mousePosition.y < boundsUpperY))
        {
            return true;
        }
        return false;
    }
}
