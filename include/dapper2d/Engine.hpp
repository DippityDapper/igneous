#pragma once

#include <cstdint>

namespace Engine
{
    class Window;
    class Renderer;
    class Scene;

    class Engine
    {
    private:
        bool running = true;

        static Scene* scene;

    public:
        Window* window = nullptr;
        Renderer* renderer = nullptr;

        uint64_t lastTick = 0;
        uint64_t currentTick = 0;
        float deltaTime = 0;

    private:
        void Render();
        void HandleEvents();
        bool InitSDL();

    public:
        void Init(Scene* _scene);
        void Update();
        void Clean();
        static void SetScene(Scene* _scene);
    };
}
