#include "dapper2d/Renderer.hpp"

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

        int spriteW = sprite->w;
        int spriteH = sprite->h;

        dest.w = (float)spriteW * mainCamera->zoom;
        dest.h = (float)spriteH * mainCamera->zoom;

        int viewportW = Window::viewport.x;
        int viewportH = Window::viewport.y;

        if (viewportW <= 0 || viewportH <= 0)
            return;

        dest.x = (position.x - mainCamera->position.x) * mainCamera->zoom + viewportW * 0.5f;
        dest.y = (position.y - mainCamera->position.y) * mainCamera->zoom + viewportH * 0.5f;

        SDL_FRect src = sprite->GetSourceRect();

        if ((dest.x > -(spriteW * mainCamera->zoom) && dest.x < viewportW) && (dest.y > -(spriteH * mainCamera->zoom) && dest.y < viewportH))
            SDL_RenderTexture(renderer, sprite->GetTexture(), &src, &dest);
    }

    void Renderer::BufferClear()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
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