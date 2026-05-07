#pragma once

#include <string>

#include "igneous/input/InputLayer.hpp"

namespace Engine
{
    class SceneRoot;

    /**
     * @class Scene
     * @brief Base class for all game scenes.
     *
     * The Scene class provides the foundation for creating game states, levels,
     * menus, and other logical divisions of your game. Scenes follow a well-defined
     * lifecycle managed by the SceneRoot.
     *
     * ## Scene Lifecycle
     *
     * When a scene is added to the SceneRoot:
     * 1. **Construction** - Scene object is created
     * 2. **InitInternal()** - Internal initialization, calls virtual Init()
     * 3. **SetActive(true/false)** - Scene is activated or left inactive
     *
     * Each frame while active:
     * 1. **HandleEventsInternal()** - Input/event processing, calls virtual HandleEvents()
     * 2. **UIInternal()** - UI processing (ImGui), calls virtual UI()
     * 3. **UpdateInternal()** - Game logic update, calls virtual Update()
     * 4. **RenderInternal()** - Rendering, calls virtual Render()
     *
     * When a scene is removed or the engine shuts down:
     * 1. **CleanInternal()** - Cleanup, calls virtual Clean()
     * 2. **Destruction** - Scene object is destroyed
     *
     * ## Active vs Inactive States
     *
     * Scenes can be active or inactive:
     * - **Active scenes**: Receive updates, render, and process input each frame
     * - **Inactive scenes**: Remain in memory but are dormant (useful for pause states)
     *
     * ## Singleton Scenes
     *
     * Scenes marked as singleton are always active and cannot be unloaded. Use this
     * for persistent systems like UI managers, audio controllers, or network handlers.
     *
     * ## Creating Custom Scenes
     *
     * To create a custom scene, inherit from Scene and override the virtual methods:
     *
     * @code
     * class GameWorld : public Engine::Scene
     * {
     * private:
     *     void Init() override
     *     {
     *         // Load resources, initialize game objects
     *     }
     *
     *     void OnActiveChanged(bool value) override
     *     {
     *         // Scene changed from active to inactive or vice versa.
     *     }
     *
     *     void Update(double delta) override
     *     {
     *         // Update game logic
     *     }
     *
     *     void Render() override
     *     {
     *         // Queue sprites/textures for rendering
     *     }
     *
     *     void UI(InputLayer& layer) override
     *     {
     *         // Render and process UI inputs.
     *         // Called before Handle Events.
     *     }
     *
     *     void HandleEvents(InputLayer& layer) override
     *     {
     *         // Process input
     *     }
     *
     *     void Clean() override
     *     {
     *         // Cleanup resources
     *     }
     * };
     * @endcode
     *
     * @note Do not override the *Internal() methods; override the virtual methods instead.
     * @note Scenes are owned by the SceneRoot and should not be manually deleted.
     *
     * @see SceneRoot
     * @see SceneManager
     */
    class Scene
    {
      public:
        /**
         * @brief Pointer to the owning SceneRoot.
         *
         * Set automatically when the scene is added to a SceneRoot via AddScene().
         * Allows scenes to access other scenes or perform scene management operations.
         *
         * @note Set by SceneRoot::AddScene().
         */
        SceneRoot* root = nullptr;

        /**
         * @brief The unique name of this scene.
         *
         * Set automatically when the scene is added to a SceneRoot via AddScene().
         * Used to identify and retrieve scenes from the SceneRoot.
         *
         * @note Must be unique within a SceneRoot.
         * @note Set by SceneRoot::AddScene().
         */
        std::string name{};

        /**
         * @brief Whether this scene is a singleton.
         *
         * Singleton scenes are always active and cannot be unloaded via
         * UnloadScene(). They are useful for persistent systems that should
         * remain active across scene transitions.
         *
         * @note Set by SceneRoot::AddScene().
         * @note Singleton scenes can still be removed via RemoveScene().
         */
        bool singleton = false;

      private:
        /**
         * @brief Whether this scene is currently active.
         *
         * Active scenes receive updates, render, and process input. Inactive
         * scenes remain in memory but are dormant.
         *
         * Modified via SetActive() and checked via IsActive().
         */
        bool active = false;

      public:
        /**
         * @brief Virtual destructor.
         *
         * Ensures proper cleanup of derived scene classes. Calls CleanInternal()
         * to guarantee cleanup occurs even if the scene is destroyed without
         * explicit removal.
         */
        virtual ~Scene();

        /**
         * @brief Internal initialization wrapper.
         *
         * Calls the virtual Init() method. Do not override this method;
         * override Init() instead.
         *
         * @note Called automatically by SceneRoot::AddScene().
         */
        void OnCreatedInternal();

        /**
         * @brief Internal update wrapper.
         *
         * Calls the virtual Update() method. Do not override this method;
         * override Update() instead.
         *
         * @param delta Time elapsed since the last frame in seconds.
         *
         * @note Called once per frame by SceneRoot::Update() if the scene is active.
         */
        void UpdateInternal(double delta);

        /**
         * @brief Internal rendering wrapper.
         *
         * Calls the virtual Render() method. Do not override this method;
         * override Render() instead.
         *
         * @note Called once per frame by SceneRoot::Render() if the scene is active.
         */
        void RenderInternal();

        /**
         * @brief Internal cleanup wrapper.
         *
         * Calls the virtual Clean() method. Do not override this method;
         * override Clean() instead.
         *
         * @note Called automatically when the scene is removed or destroyed.
         */
        void OnDestroyInternal();

        /**
         * @brief Internal event handling wrapper.
         *
         * Calls the virtual HandleEvents() method. Do not override this method;
         * override HandleEvents() instead.
         *
         * @param layer The input layer for event processing.
         *
         * @note Called once per frame by SceneRoot::HandleEvents() if the scene is active.
         */
        void HandleInputsInternal(InputLayer& layer);

        /**
         * @brief Sets the active state of the scene.
         *
         * When a scene's active state changes, the virtual OnActiveChanged()
         * method is called. This allows scenes to respond to activation/deactivation
         * (e.g., pausing audio, saving state).
         *
         * @param value true to activate the scene, false to deactivate it.
         *
         * @note If the value is the same as the current state, this is a no-op.
         * @note Calls OnActiveChanged() when the state actually changes.
         */
        void SetActive(bool value);

        /**
         * @brief Checks if the scene is currently active.
         *
         * @return true if the scene is active (receives updates/renders), false otherwise.
         */
        bool IsActive();

      private:
        /**
         * @brief Called when the scene is initialized.
         *
         * Override this method to initialize your scene's resources, load assets,
         * create game objects, and set up initial state.
         *
         * @note Called once when the scene is added to the SceneRoot.
         * @note Called before the scene is set to active.
         */
        virtual void OnCreated()
        {
        }

        /**
         * @brief Called every frame to update game logic.
         *
         * Override this method to update your scene's game logic.
         *
         * @param delta Time elapsed since the last frame in seconds.
         *
         * @note Only called when the scene is active.
         * @note Do not perform rendering in this method; use Render() instead.
         */
        virtual void Update(double delta)
        {
        }

        /**
         * @brief Called every frame to render the scene.
         *
         * Override this method to manually queue sprites and textures for rendering via
         * Renderer::BufferAdd().
         *
         * @note Only called when the scene is active.
         * @note Rendering order matches scene iteration order in SceneRoot.
         * @note Sprites are automatically rendered via the ResourceManager.
         */
        virtual void Render()
        {
        }

        /**
         * @brief Called every frame to handle input events.
         *
         * Override this method to process input events from the given input layer,
         * such as keyboard, mouse, or gamepad input.
         *
         * @param layer The input layer containing input state and events.
         *
         * @note Only called when the scene is active.
         * @note Called before Update() each frame.
         *
         * @see InputLayer
         */
        virtual void HandleInputs(InputLayer& layer)
        {
        }

        /**
         * @brief Called when the scene is being destroyed.
         *
         * Override this method to clean up your scene's resources, including:
         * - Releasing loaded assets
         * - Destroying game objects
         * - Clearing references
         *
         * @note Called automatically when the scene is removed from the SceneRoot.
         * @note Also called by the destructor as a safety measure.
         */
        virtual void OnDestroy()
        {
        }

        /**
         * @brief Called when the scene's active state changes.
         *
         * Override this method to respond to activation/deactivation, such as:
         * - Pausing/resuming audio
         * - Saving/loading state
         * - Showing/hiding UI elements
         * - Starting/stopping background processes
         *
         * @param value The new active state (true = activated, false = deactivated).
         *
         * @note Only called when the active state actually changes.
         * @note Not called during initialization or cleanup.
         */
        virtual void OnActiveChanged(bool value)
        {
        }
    };
}