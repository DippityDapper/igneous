#pragma once

#include "igneous/scenes/Scene.hpp"
#include "igneous/scenes/SceneManager.hpp"

// Using Doxygen-style documentation

namespace Engine
{
    /**
     * @class Engine
     * @brief The main engine class that manages the application lifecycle.
     *
     * The Engine class is responsible for initialization, the main update/render loop,
     * and cleanup of all engine subsystems. It owns the renderer, window, and manages
     * scene transitions through the SceneManager.
     *
     * The engine initializes and manages the following subsystems:
     * - SDL3 (video and audio)
     * - SDL_mixer (audio mixing)
     * - ENet (networking)
     * - ImGui (UI)
     * - Input system
     * - Scene management
     * - Resource management
     * - Time/delta time tracking
     *
     * @note Only one Engine instance should exist at a time.
     * @note The engine takes ownership of scenes and cleans them up appropriately.
     *
     * Example usage:
     * @code
     * auto engine = std::make_unique<Engine::Engine>();
     * return engine->Run<Game::World>("MainWorld");
     * @endcode
     */
    class Engine
    {
      private:
        /**
         * @brief Flag indicating whether the engine is currently running.
         *
         * When set to false, the engine will exit the main loop at the next update cycle.
         * Modified by the Quit() method.
         */
        static inline bool running = true;

      private:
        /**
         * @brief Handles SDL and ImGui events, and distributes input to active scenes.
         *
         * This method:
         * - Resets per-frame input state
         * - Polls all SDL events (window, input, etc.)
         * - Processes ImGui events
         * - Handles window resize events
         * - Handles quit events
         * - Distributes input events to all active input layers
         * - Calls UI(), HandleEvents(), and camera event handling for the scene hierarchy
         *
         * @note Called once per frame as part of the Update() loop.
         * @note Event processing order: Scene UI → Camera → Scene event handlers
         */
        void HandleEvents();

        /**
         * @brief Initializes SDL3, SDL_mixer, the window, renderer, and time tracking.
         *
         * Initializes:
         * - SDL3 video and audio subsystems
         * - SDL_mixer for audio
         * - Window with default size (640x360)
         * - Renderer
         * - Time tracking (currentTick and lastTick)
         *
         * @return true if initialization succeeded, false otherwise.
         * @note Logs errors to SDL_Log on failure.
         * @note Called during Init().
         */
        bool InitSDL() const;

        /**
         * @brief Initializes the ENet networking library.
         *
         * @return true if initialization succeeded, false otherwise.
         * @note Logs errors to SDL_Log on failure.
         * @note Called during Init().
         */
        bool InitENet() const;

        /**
         * @brief Initializes all engine subsystems.
         *
         * Calls initialization functions for:
         * - SDL and related systems (InitSDL)
         * - ENet networking (InitENet)
         * - Input system
         * - Scene manager
         *
         * If any subsystem fails to initialize, sets running to false.
         *
         * @note Called automatically by Run() before the main loop starts.
         */
        void Init();

        /**
         * @brief The main game loop.
         *
         * Each frame performs the following operations in order:
         * 1. Clears the render buffer
         * 2. Updates time tracking (delta time calculation)
         * 3. Handles events (input, window, etc.)
         * 4. Updates the scene hierarchy
         * 5. Updates the main camera
         * 6. Cleans expired resources (up to 10 per frame)
         * 7. Renders the scene hierarchy
         * 8. Renders all sprites from the resource manager
         * 9. Presents the final rendered frame
         *
         * Continues until running is set to false.
         *
         * @note Called automatically by Run() after initialization.
         */
        void Update();

        /**
         * @brief Cleans up all engine resources and shuts down subsystems.
         *
         * Cleanup order:
         * 1. Scene hierarchy cleanup
         * 2. Resource manager cleanup
         * 3. ENet deinitialization
         * 4. Renderer cleanup
         * 5. Window cleanup
         * 6. SDL shutdown
         *
         * @note Called automatically by Run() after the main loop exits.
         */
        void Clean() const;

      public:
        /**
         * @brief Starts the engine with an initial scene.
         *
         * This is the main entry point for running the engine. It performs the following:
         * 1. Initializes all engine subsystems (Init)
         * 2. Creates and activates the initial scene of type T
         * 3. Runs the main loop (Update)
         * 4. Cleans up all resources (Clean)
         *
         * @tparam T The scene type to instantiate. Must derive from Scene.
         * @param initialSceneName The name to assign to the initial scene.
         * @return 0 on successful completion.
         *
         * @note This method blocks until the engine shuts down.
         * @note The scene is added to the scene root as active and persistent.
         *
         * Example:
         * @code
         * engine->Run<MainMenuScene>("MainMenu");
         * @endcode
         */
        template<class T>
        requires(std::is_base_of_v<Scene, T>)
        int Run(const std::string& initialSceneName);

        /**
         * @brief Signals the engine to stop running.
         *
         * Sets the running flag to false, which causes the main loop to exit
         * after the current frame completes. Does not immediately terminate
         * the application; cleanup still occurs normally.
         *
         * @note Can be called from anywhere, including event handlers and scene code.
         * @note Thread-safe due to static inline bool implementation.
         */
        static void Quit();
    };

    template<class T>
    requires(std::is_base_of_v<Scene, T>)
    int Engine::Run(const std::string& initialSceneName)
    {
        Init();
        SceneManager::GetSceneRoot()->AddScene<T>(initialSceneName, true, false);
        Update();
        Clean();
        return 0;
    }
}
