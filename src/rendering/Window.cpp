#include "igneous/rendering/Window.hpp"

#include "SDL3/SDL.h"

namespace Engine
{
    namespace
    {
        bool wasResized = false;
    }

    SDL_Window* Window::window = nullptr;
    Vec2<int> Window::viewport{0, 0};

    void Engine::Window::Init(int w, int h, const std::string& title)
    {
        window = SDL_CreateWindow(title.c_str(), w, h, SDL_WINDOW_RESIZABLE);
        viewport.x = w;
        viewport.y = h;
        wasResized = false;
    }

    void Window::OnResize(int w, int h)
    {
        viewport.x = w;
        viewport.y = h;
        wasResized = true;
    }

    bool Window::WasResized()
    {
        return wasResized;
    }

    void Window::ResetFrameState()
    {
        wasResized = false;
    }

    void Window::Clean()
    {
        if (window)
        {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
    }

    void Window::ResetForTests()
    {
        Clean();
        viewport = {0, 0};
        wasResized = false;
    }

    SDL_Window* Window::GetWindow()
    {
        return window;
    }
}
