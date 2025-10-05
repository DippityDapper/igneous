#pragma once

struct SDL_Renderer;

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
        static SDL_Renderer* GetRenderer();
    };
}
