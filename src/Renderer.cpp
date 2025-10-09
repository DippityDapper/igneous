#include "dapper2d/Renderer.hpp"

#include "SDL3/SDL.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

#include "dapper2d/Vec2.hpp"
#include "dapper2d/Sprite.hpp"
#include "dapper2d/Camera.hpp"
#include "dapper2d/Window.hpp"

namespace Engine
{
    SDL_Renderer* Renderer::renderer = nullptr;

    void Renderer::Init()
    {
        if (!Window::GetWindow())
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to find window for renderer");
            return;
        }

        renderer = SDL_CreateRenderer(Window::GetWindow(), nullptr);

        if (!renderer)
            SDL_Log("Error making renderer: %s", SDL_GetError());

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForSDLRenderer(Window::GetWindow(), renderer);
        ImGui_ImplSDLRenderer3_Init(renderer);
    }

    void Renderer::Render()
    {
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    void Renderer::BufferAdd(Vec2<float> position, Sprite* sprite)
    {
        Camera* mainCamera = Camera::main;

        if (!mainCamera || !renderer || !sprite || !sprite->GetTexture())
            return;

        SDL_FRect dest;

        int spriteW = sprite->tileW;
        int spriteH = sprite->tileH;

        dest.w = (float)spriteW * mainCamera->zoom;
        dest.h = (float)spriteH * mainCamera->zoom;

        int viewportW = Window::viewport.x;
        int viewportH = Window::viewport.y;

        if (viewportW <= 0 || viewportH <= 0)
            return;

        dest.x = (position.x - mainCamera->position.x) * mainCamera->zoom + viewportW * 0.5f;
        dest.y = (position.y - mainCamera->position.y) * mainCamera->zoom + viewportH * 0.5f;

        SDL_FRect src = sprite->GetSourceRect();
        SDL_RenderTexture(renderer, sprite->GetTexture(), &src, &dest);
    }

    void Renderer::BufferAdd(Vec2<float> position, SDL_Texture* texture)
    {
        if (!texture)
            return;

        Engine::Camera* camera = Engine::Camera::main;
        float zoom = camera->zoom;

        SDL_FRect dest;

        dest.w = texture->w * zoom;
        dest.h = texture->h * zoom;

        int viewportW = Engine::Window::viewport.x;
        int viewportH = Engine::Window::viewport.y;

        if (viewportW <= 0 || viewportH <= 0)
            return;

        dest.x = (position.x - camera->position.x) * zoom + viewportW * 0.5f;
        dest.y = (position.y - camera->position.y) * zoom + viewportH * 0.5f;

        SDL_RenderTexture(renderer, texture, nullptr, &dest);
    }

    void Renderer::BufferAddNoOffset(Vec2<float> position, Sprite* sprite)
    {
        if (!sprite || !sprite->GetTexture())
            return;

        SDL_FRect dest;

        dest.w = sprite->tileW;
        dest.h = sprite->tileH;

        dest.x = position.x;
        dest.y = position.y;

        SDL_FRect src = sprite->GetSourceRect();
        SDL_RenderTexture(renderer, sprite->GetTexture(), &src, &dest);
    }

    void Renderer::BufferClear()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    SDL_Renderer* Renderer::GetRenderer()
    {
        return renderer;
    }

    void Renderer::Clean()
    {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        SDL_DestroyRenderer(renderer);
    }
}