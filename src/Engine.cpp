#include "dapper2d/Engine.hpp"

namespace Engine
{
    Scene* Engine::scene = nullptr;

    void Engine::Init(Scene* _scene)
    {
        if (InitSDL() == SDL_APP_FAILURE)
        {
            running = false;
            return;
        }

        SetScene(_scene);
    }

    SDL_AppResult Engine::InitSDL()
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_Log("SDL init failed: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        window.Init(640, 360);
        renderer.Init();

        currentTick = SDL_GetTicks();
        lastTick = currentTick;

        return SDL_APP_CONTINUE;
    }

    void Engine::HandleEvents()
    {
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent))
        {
            ImGui_ImplSDL3_ProcessEvent(&sdlEvent);

            if (sdlEvent.type == SDL_EVENT_QUIT)
            {
                running = false;
                return;
            }
            if (sdlEvent.type == SDL_EVENT_KEY_DOWN)
            {
                if (sdlEvent.key.key == SDLK_ESCAPE)
                {
                    running = false;
                    return;
                }
            }
            if (sdlEvent.type == SDL_EVENT_WINDOW_RESIZED)
            {
                Window::viewport.x = sdlEvent.window.data1;
                Window::viewport.y = sdlEvent.window.data2;
            }
            if (Camera::main)
                    Camera::main->HandleEvents(sdlEvent);
            if (scene)
                scene->HandleEventsInternal(sdlEvent);
        }
    }

    void Engine::Update()
    {
        while (running)
        {
            HandleEvents();

            lastTick = currentTick;
            currentTick = SDL_GetTicks();
            deltaTime = (float)(currentTick - lastTick) / 1000.0f;

            if (Camera::main)
                Camera::main->Update(deltaTime);
            if (scene)
                scene->UpdateInternal(deltaTime);

            ResourceLoader::CleanExpired(10);
            Render();
        }
    }

    void Engine::Render()
    {
        Renderer::BufferClear();
        if (scene)
            scene->RenderInternal();
        renderer.Render();
    }

    void Engine::Clean()
    {
        if (scene)
        {
            scene->CleanInternal();
            delete scene;
        }

        renderer.Clean();
        window.Clean();

        SDL_Quit();
    }

    void Engine::SetScene(Scene* _scene)
    {
        if (scene)
        {
            scene->CleanInternal();
            delete scene;
        }

        scene = _scene;
        scene->InitInternal();
    }
}
