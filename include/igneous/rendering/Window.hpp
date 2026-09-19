#pragma once

#include <string>

#include "igneous/engine/Vec2.hpp"

struct SDL_Window;

namespace Engine
{

    class Window
    {
      private:

        static SDL_Window* window;

      public:

        static Vec2<int> viewport;

      public:

        static void Init(int w, int h, const std::string& title = "client");

        static void Clean();

        static void OnResize(int w, int h);

        static bool WasResized();

        static void ResetFrameState();

        static void ResetForTests();

        static SDL_Window* GetWindow();
    };
}
