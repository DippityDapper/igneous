#pragma once

#include <cstdint>

namespace Engine
{
    class Window;
    class Renderer;
    class Scene;

    /// The main engine class.
    ///
    /// The `Engine` manages the application lifecycle — initialization, the main update/render loop,
    /// and cleanup. It owns the renderer, window, and currently active scene.
    ///
    /// Typical usage:
    /// @code
    /// auto engine = std::make_unique<Engine::Engine>();
    /// auto scene  = Game::World();
    /// return engine->Run(scene);
    /// @endcode
    ///
    /// Scenes are cleaned by the engine when a new scene is set.
    /// Only one `Engine` instance should exist at a time.
    class Engine
    {
    private:
        static bool running;

        static uint64_t lastTick;
        static uint64_t currentTick;
        static float deltaTime;

    public:
        /// Pointer to the window used for rendering.
        /// @see Engine::Window
        Window* window = nullptr;

        /// Pointer to the renderer handling all draw calls.
        /// @see Engine::Renderer
        Renderer* renderer = nullptr;

    private:
        /// Handles rendering.
        /// @note This is called as part of the Engine update loop.
        void Render() const;

        /// Handles SDL and ImGui events.
        /// @note This is called as part of the Engine update loop.
        void HandleEvents();

        /// Initializes SDL, the renderer, and the window.
        /// @note This is called as part of the Engine initialization.
        bool InitSDL() const;

        /// Initializes ENet.
        /// @note This is called as part of the Engine initialization.
        bool InitENet() const;

        /// Initializes SDL and sets the current scene.
        /// @param scene The initial scene to be updated and rendered.
        void Init();

        /// The main game loop. Handles events, rendering, and updates.
        void Update();
        
        /// Cleans various parts of the engine, such as textures, the renderer, the window, and the current scene.
        void Clean() const;

    public:
        /// Starts the main loop given an initial scene.
        /// @param scene The initial scene to be updated and rendered.
        /// @returns The error code.
        int Run();

        /// Gets the delta time from the last frame to the current frame.
        /// @returns The delta time.
        static float GetDeltaTime();

        /// Signals the main loop to stop running at the next update cycle.
        /// Does not immediately exit the application.
        static void Quit();
    };
}
