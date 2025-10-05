#include "dapper2d/Window.hpp"

#include "SDL3/SDL.h"

namespace Engine
{
    SDL_Window* Window::window = nullptr;
    Vec2<int> Window::viewport{0,0};

    void Engine::Window::Init(int w, int h)
    {
        window = SDL_CreateWindow("client", w, h, SDL_WINDOW_RESIZABLE);
        viewport.x = w;
        viewport.y = h;
    }

    void Window::Clean()
    {
        SDL_DestroyWindow(window);
    }

    SDL_Window* Window::GetWindow()
    {
        return window;
    }
}