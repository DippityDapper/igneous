#pragma once

#include "igneous/engine/Vec2.hpp"

struct SDL_Window;

namespace Engine
{
    /**
     * @class Window
     * @brief Manages the SDL3 window and viewport dimensions.
     *
     * The Window class provides a centralized interface for window management,
     * including creation, cleanup, and viewport tracking. The viewport dimensions
     * are automatically updated when the window is resized.
     *
     * The window is created as resizable and titled "client" by default.
     * Viewport dimensions are stored separately from the window to allow for
     * efficient access during rendering operations.
     *
     * @note This class uses static members and should not be copied.
     * @note Only one window should exist at a time.
     * @note The viewport is automatically updated by the Engine when SDL_EVENT_WINDOW_RESIZED occurs.
     */
    class Window
    {
    private:
        /**
         * @brief The static SDL3 window instance.
         *
         * Created during Init() and destroyed during Clean().
         * Initialized to nullptr.
         */
        static SDL_Window* window;

    public:
        /**
         * @brief The current viewport dimensions in pixels.
         *
         * Represents the window's drawable area size. This is set during Init()
         * and automatically updated by the Engine when the window is resized
         * (SDL_EVENT_WINDOW_RESIZED event).
         *
         * Used by the Renderer for calculating screen-space coordinates and
         * by the Camera for world-to-screen transformations.
         *
         * @note Accessed directly for performance during rendering.
         * @note Initialized to {0, 0} before Init() is called.
         */
        static Vec2<int> viewport;

    public:
        /**
         * @brief Initializes the SDL3 window with the specified dimensions.
         *
         * Creates a resizable SDL window titled "client" with the given width
         * and height, then sets the viewport to match these dimensions.
         *
         * @param w Initial window width in pixels.
         * @param h Initial window height in pixels.
         *
         * @note The window is created with the SDL_WINDOW_RESIZABLE flag.
         * @note The window title is hard-coded as "client".
         * @note Called automatically by the Engine during SDL initialization.
         * @note Must be called before Renderer::Init().
         */
        static void Init(int w, int h);

        /**
         * @brief Destroys the SDL3 window.
         *
         * Cleans up the window resources by calling SDL_DestroyWindow.
         *
         * @note Called automatically by the Engine during shutdown.
         * @note Should be called after Renderer::Clean().
         */
        static void Clean();

        /**
         * @brief Gets the static SDL_Window instance.
         *
         * Provides access to the underlying SDL window for operations not
         * covered by the Window class interface.
         *
         * @return Pointer to the SDL_Window, or nullptr if not initialized.
         *
         * @note Required by Renderer::Init() to create the SDL renderer.
         */
        static SDL_Window* GetWindow();
    };
}