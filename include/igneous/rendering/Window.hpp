#pragma once

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

        static void Init(int w, int h);

        static void Clean();

        /// Clears static window state between unit tests.
        static void ResetForTests();

        static SDL_Window* GetWindow();
    };
}
