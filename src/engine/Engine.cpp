#include "engine/engine/Engine.h"

namespace Engine
{
    void Engine::Init()
    {
        if (InitSDL() == SDL_APP_FAILURE)
        {
            running = false;
            return;
        }

//        if (InitNetworking() == SDL_APP_FAILURE)
//        {
//            running = false;
//            return;
//        }

        Game::World* world = new Game::World();
        SetScene(world);
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

    SDL_AppResult Engine::InitNetworking()
    {
        if (networkManager.Init() == SDL_APP_FAILURE)
        {
            return SDL_APP_FAILURE;
        }
        if (networkManager.CreateClient() == SDL_APP_FAILURE)
        {
            return SDL_APP_FAILURE;
        }

        CFGParser::LoadConfig("server-properties.cfg");

        std::string ip = CFGParser::GetString("server-properties.cfg", "server-ip");
        int port = CFGParser::GetInt("server-properties.cfg", "server-port");

        if (networkManager.ConnectToServer(port, ip) == SDL_APP_FAILURE)
        {
            return SDL_APP_FAILURE;
        }

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
                if (networkManager.clientId >= 0)
                    networkManager.Disconnect();
                return;
            }
            if (sdlEvent.type == SDL_EVENT_KEY_DOWN)
            {
                if (sdlEvent.key.key == SDLK_ESCAPE)
                {
                    running = false;
                    if (networkManager.clientId >= 0)
                        networkManager.Disconnect();
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

        ResourceLoader::UnloadAll();

        renderer.Clean();
        window.Clean();
        networkManager.Clean();

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
