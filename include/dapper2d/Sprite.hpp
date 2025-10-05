#pragma once

#include <string>
#include <memory>

struct SDL_Texture;
struct SDL_FRect;

namespace Engine
{
    class Sprite
    {
    private:
        std::shared_ptr<SDL_Texture> texture = nullptr;

    public:
        float w = 0;
        float h = 0;

        float tileW = 0;
        float tileH = 0;

        int x = 0;
        int y = 0;

    public:
        explicit Sprite(std::string& texturePath);
        Sprite(std::string& texturePath, float _w, float _h, int _x, int _y);

        SDL_Texture* GetTexture();
        SDL_FRect GetSourceRect();
    };
}
