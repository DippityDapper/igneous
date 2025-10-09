#pragma once

struct SDL_Renderer;
struct SDL_Texture;

namespace Engine
{
    template<typename T>
    struct Vec2;
    class Sprite;

    class Renderer
    {
    private:
        static SDL_Renderer* renderer;

    public:
        void Init();
        void Render();
        void Clean();

        static void BufferClear();
        static void BufferAdd(Vec2<float> position, Sprite* sprite);
        static void BufferAdd(Vec2<float> position, SDL_Texture* texture);
        static void BufferAddNoOffset(Vec2<float> position, Sprite* sprite);
        static SDL_Renderer* GetRenderer();
    };
}
