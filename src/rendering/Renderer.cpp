#include "igneous/rendering/Renderer.hpp"

#include "SDL3/SDL.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

#include "igneous/engine/Vec2.hpp"
#include "igneous/resources/Sprite.hpp"
#include "igneous/engine/Camera.hpp"
#include "igneous/rendering/Window.hpp"

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
        (void) io;
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

    void Renderer::BufferAdd(Vec2<float> position, Sprite* sprite, Camera* camera)
    {
        if (camera == nullptr)
        {
            if (Camera::main == nullptr)
            {
                BufferAdd(position, sprite);
                return;
            }

            camera = Camera::main;
        }

        if (!renderer || !sprite || !sprite->GetTexture())
            return;

        SDL_FRect dest;

        Animation* currentAnimation = sprite->GetCurrentAnimation();
        if (!currentAnimation)
            return;

        int spriteW = currentAnimation->GetCurrentFrame()->atlasW;
        int spriteH = currentAnimation->GetCurrentFrame()->atlasH;
        float scaleX = sprite->scaleX;
        float scaleY = sprite->scaleY;

        dest.w = (float) spriteW * camera->zoom * scaleX;
        dest.h = (float) spriteH * camera->zoom * scaleY;

        int viewportW = Window::viewport.x;
        int viewportH = Window::viewport.y;

        if (viewportW <= 0 || viewportH <= 0)
            return;

        float screenX = (position.x - camera->position.x) * camera->zoom + viewportW * 0.5f;
        float screenY = (position.y - camera->position.y) * camera->zoom + viewportH * 0.5f;

        if (sprite->centered)
        {
            dest.x = screenX - dest.w * 0.5f;
            dest.y = screenY - dest.h * 0.5f;
        }
        else
        {
            dest.x = screenX;
            dest.y = screenY;
        }

        SDL_FRect src = sprite->GetSourceRect();
        SDL_RenderTexture(renderer, sprite->GetTexture(), &src, &dest);
    }

    void Renderer::BufferAdd(Vec2<float> position, SDL_Texture* texture, Camera* camera, bool centered)
    {
        if (!texture)
            return;

        if (camera == nullptr)
        {
            if (Camera::main == nullptr)
                BufferAdd(position, texture, centered);

            camera = Camera::main;
        }

        float zoom = camera->zoom;

        SDL_FRect dest;

        dest.w = texture->w * zoom;
        dest.h = texture->h * zoom;

        int viewportW = Window::viewport.x;
        int viewportH = Window::viewport.y;

        if (viewportW <= 0 || viewportH <= 0)
            return;

        float screenX = (position.x - camera->position.x) * camera->zoom + viewportW * 0.5f;
        float screenY = (position.y - camera->position.y) * camera->zoom + viewportH * 0.5f;

        if (centered)
        {
            dest.x = screenX - dest.w * 0.5f;
            dest.y = screenY - dest.h * 0.5f;
        }
        else
        {
            dest.x = screenX;
            dest.y = screenY;
        }

        SDL_RenderTexture(renderer, texture, nullptr, &dest);
    }

    void Renderer::BufferAdd(Vec2<float> position, Sprite* sprite)
    {
        if (!sprite || !sprite->GetTexture())
            return;

        SDL_FRect dest;

        Animation* currentAnimation = sprite->GetCurrentAnimation();
        if (!currentAnimation)
            return;

        int spriteW = currentAnimation->GetCurrentFrame()->atlasW;
        int spriteH = currentAnimation->GetCurrentFrame()->atlasH;
        float scaleX = sprite->scaleX;
        float scaleY = sprite->scaleY;

        dest.w = spriteW * scaleX;
        dest.h = spriteH * scaleY;

        dest.x = position.x;
        dest.y = position.y;

        if (sprite->centered)
        {
            dest.x = position.x - dest.w * 0.5f;
            dest.y = position.y - dest.h * 0.5f;
        }
        else
        {
            dest.x = position.x;
            dest.y = position.y;
        }

        SDL_FRect src = sprite->GetSourceRect();
        SDL_RenderTexture(renderer, sprite->GetTexture(), &src, &dest);
    }
    void Renderer::BufferAdd(Vec2<float> position, SDL_Texture* texture, bool centered)
    {
        if (!texture)
            return;

        SDL_FRect dest;

        dest.w = texture->w;
        dest.h = texture->h;

        int viewportW = Window::viewport.x;
        int viewportH = Window::viewport.y;

        if (viewportW <= 0 || viewportH <= 0)
            return;

        float screenX = position.x;
        float screenY = position.y;

        if (centered)
        {
            dest.x = screenX - dest.w * 0.5f;
            dest.y = screenY - dest.h * 0.5f;
        }
        else
        {
            dest.x = screenX;
            dest.y = screenY;
        }

        SDL_RenderTexture(renderer, texture, nullptr, &dest);
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