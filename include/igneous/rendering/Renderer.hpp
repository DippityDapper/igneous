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

        Renderer(const Renderer& other) = delete;

        static void Init();

        static void Render();

        static void Clean();

        /// Clears static renderer state between unit tests.
        static void ResetForTests();

        static void BufferClear();

        static void BufferAdd(Vec2<float> position, Sprite* sprite, Camera* camera);

        static void BufferAdd(Vec2<float> position, SDL_Texture* texture, Camera* camera, bool centered = true);

        static void BufferAdd(Vec2<float> position, Sprite* sprite);

        static void BufferAdd(Vec2<float> position, SDL_Texture* texture, bool centered = true);

        static SDL_Renderer* GetRenderer();
    };
}
