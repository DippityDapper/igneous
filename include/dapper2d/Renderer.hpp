#pragma once

struct SDL_Renderer;
struct SDL_Texture;

namespace Engine
{
    template<typename T>
    struct Vec2;
    class Sprite;
    class Camera;

    class Renderer
    {
    private:
        static SDL_Renderer* renderer;

    public:
        void Init();
        void Render();
        void Clean();

        static void BufferClear();
        static void BufferAdd(Vec2<float> position, Sprite* sprite, Camera* camera);
        static void BufferAdd(Vec2<float> position, SDL_Texture* texture, Camera* camera);
        static void BufferAdd(Vec2<float> position, Sprite* sprite);
        static SDL_Renderer* GetRenderer();
    };
}
