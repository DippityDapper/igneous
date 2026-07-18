// Doc: docs/classes/Window.md
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

        /// Updates viewport dimensions and marks the window resized for this frame.
        static void OnResize(int w, int h);

        /// True if a resize occurred since the last `ResetFrameState()` call.
        static bool WasResized();

        /// Clears per-frame resize state at the start of event processing.
        static void ResetFrameState();

        /// Clears static window state between unit tests.
        static void ResetForTests();

        static SDL_Window* GetWindow();
    };
}
