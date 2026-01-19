#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "SDL3/SDL_log.h"
#include "igneous/scenes/Scene.hpp"

namespace Engine
{
    /**
     * @class SceneRoot
     * @brief The top-level container for all scenes in the engine.
     *
     * The SceneRoot manages a collection of named scenes and their lifecycle.
     * It handles scene creation, activation/deactivation, and propagates engine
     * events (update, render, input) to all active scenes.
     *
     * Scenes can be in one of two states:
     * - **Active**: Receives updates, renders, and processes input
     * - **Inactive**: Exists in memory but does not update or render
     *
     * Scenes can be marked as "singleton" scenes, which are always active and
     * cannot be unloaded. This is useful for persistent systems like UI managers,
     * audio systems, or network handlers.
     *
     * Scene management operations include:
     * - Adding new scenes with AddScene()
     * - Loading/unloading scenes to control which are active
     * - Removing scenes entirely from memory
     *
     * @note Scene names must be unique within the SceneRoot.
     * @note Singleton scenes cannot be unloaded (deactivated).
     * @note The SceneRoot is owned and managed by the SceneManager.
     *
     * @see Scene
     * @see SceneManager
     */
    class SceneRoot
    {
      private:
        /**
         * @brief Map of scene names to scene instances.
         *
         * Stores all scenes (both active and inactive) by their unique names.
         * The SceneRoot owns all scene instances through unique_ptr.
         */
        std::unordered_map<std::string, std::unique_ptr<Scene>> scenes{};

      public:
        /**
         * @brief Initializes the SceneRoot.
         *
         * Currently a placeholder for future initialization logic.
         *
         * @note Called automatically by SceneManager::SetSceneRoot().
         */
        void Init();

        /**
         * @brief Updates all active scenes.
         *
         * Iterates through all scenes and calls UpdateInternal() on each active
         * scene, passing the frame delta time.
         *
         * @param delta Time elapsed since the last frame in seconds.
         *
         * @note Only active scenes are updated.
         * @note Called once per frame by the Engine.
         * @note Scenes are updated in insertion order (map iteration order).
         */
        void Update(float delta);

        /**
         * @brief Processes UI for all active scenes.
         *
         * Iterates through all scenes and calls UIInternal() on each active
         * scene for the given input layer. Used for ImGui and other UI systems.
         *
         * @param layer The input layer to process UI for.
         *
         * @note Only active scenes process UI.
         * @note Called once per frame by the Engine during event handling.
         * @note Scenes process UI in insertion order.
         *
         * @see InputLayer
         */
        void UI(InputLayer& layer);

        /**
         * @brief Renders all active scenes.
         *
         * Iterates through all scenes and calls RenderInternal() on each active
         * scene. Used to manually render sprites, textures, and ImGui. Sprites
         * are automatically rendered from the ResourceManager.
         *
         * @note Only active scenes are rendered.
         * @note Called once per frame by the Engine.
         * @note Scenes are rendered in insertion order (affects draw order).
         */
        void Render();

        /**
         * @brief Handles input events for all active scenes.
         *
         * Iterates through all scenes and calls HandleEventsInternal() on each
         * active scene for the given input layer.
         *
         * @param layer The input layer to process events for.
         *
         * @note Only active scenes process events.
         * @note Called once per frame by the Engine during event handling.
         * @note Scenes handle events in insertion order.
         *
         * @see InputLayer
         */
        void HandleEvents(InputLayer& layer);

        /**
         * @brief Cleans up all scenes and clears the scene map.
         *
         * Destroys all scene instances, releasing their resources and
         * calling CleanInternal() on all existing scenes.
         *
         * @note Called automatically by SceneManager::RemoveSceneRoot().
         * @note Called automatically by the Engine during shutdown.
         */
        void Clean();

        /**
         * @brief Adds a new scene to the SceneRoot.
         *
         * Creates a scene of type T with the specified name and configuration.
         * The scene is initialized via InitInternal() and can optionally be set
         * as active immediately.
         *
         * Singleton scenes are always set to active regardless of the active
         * parameter. Singleton scenes cannot be unloaded and are useful for
         * persistent systems.
         *
         * @tparam T The scene type to create. Must derive from Scene.
         * @param name Unique name for the scene. Must not already exist.
         * @param active Whether the scene should be active initially. Default is true.
         *               Ignored for singleton scenes (always true).
         * @param singleton Whether this scene is a singleton (always active, cannot
         *                  be unloaded). Default is false.
         *
         * @return Pointer to the newly created scene, or nullptr if a scene with
         *         the same name already exists.
         *
         * @note Scene names must be unique.
         * @note Singleton scenes are always active and cannot be unloaded.
         * @note The SceneRoot takes ownership of the created scene.
         *
         * Example:
         * @code
         * sceneRoot->AddScene<GameWorld>("MainWorld", true, false);
         * sceneRoot->AddScene<UIManager>("UI", true, true); // Singleton
         * @endcode
         */
        template<class T>
        requires(std::is_base_of_v<Scene, T>)
        T* AddScene(const std::string& name, bool active = true, bool singleton = false);

        /**
         * @brief Removes a scene from the SceneRoot.
         *
         * Calls CleanInternal() on the scene before removing it from the map.
         * This permanently destroys the scene and frees its resources.
         *
         * @param name The name of the scene to remove.
         *
         * @note Logs an error if the scene does not exist.
         * @note Can remove both active and inactive scenes.
         * @note Can remove singleton scenes (unlike UnloadScene).
         */
        void RemoveScene(const std::string& name);

        /**
         * @brief Loads (activates) a scene by name.
         *
         * Sets the specified scene to active. Optionally unloads all other
         * non-singleton scenes before loading the target scene. This is useful
         * for transitioning between game states (e.g., main menu to gameplay).
         *
         * Singleton scenes are never unloaded, even when unloadAll is true.
         *
         * @param name The name of the scene to load.
         * @param unloadAll If true, deactivates all non-singleton scenes before
         *                  loading the target scene. Default is true.
         *
         * @return true if the scene was loaded successfully, false if the scene
         *         does not exist.
         *
         * @note Logs an error if the scene does not exist.
         * @note Singleton scenes remain active regardless of unloadAll.
         *
         * Example:
         * @code
         * // Load gameplay, unloading menu
         * sceneRoot->LoadScene("Gameplay", true);
         *
         * // Load pause menu without unloading gameplay
         * sceneRoot->LoadScene("PauseMenu", false);
         * @endcode
         */
        bool LoadScene(const std::string& name, bool unloadAll = true);

        /**
         * @brief Unloads (deactivates) a scene by name.
         *
         * Sets the specified scene to inactive. The scene remains in memory but
         * stops receiving updates, rendering, and input events.
         *
         * Singleton scenes cannot be unloaded and will cause this method to fail.
         *
         * @param name The name of the scene to unload.
         *
         * @return true if the scene was unloaded successfully, false if the scene
         *         does not exist or is a singleton.
         *
         * @note Logs an error if the scene does not exist or is a singleton.
         * @note To permanently remove a scene, use RemoveScene() instead.
         */
        bool UnloadScene(const std::string& name);

        /**
         * @brief Unloads all non-singleton scenes.
         *
         * Deactivates all scenes that are not marked as singletons. Singleton
         * scenes remain active. This is useful for transitioning to a "neutral"
         * state before loading new scenes.
         *
         * @note Singleton scenes are never unloaded.
         * @note Scenes remain in memory; use RemoveScene() to permanently delete.
         */
        void UnloadAllScenes();

        /**
         * @brief Gets a pointer to a scene by name.
         *
         * Retrieves a scene from the scene map, regardless of whether it is
         * active or inactive.
         *
         * @param name The name of the scene to retrieve.
         *
         * @return Pointer to the scene, or nullptr if the scene does not exist.
         *
         * @note Logs an error if the scene does not exist.
         * @note Check for nullptr before use.
         */
        Scene* GetScene(const std::string& name);

        /**
         * @brief Checks if a scene exists in the SceneRoot.
         *
         * @param name The name of the scene to check.
         *
         * @return true if the scene exists (active or inactive), false otherwise.
         */
        bool SceneExists(const std::string& name);
    };

    template<class T>
    requires(std::is_base_of_v<Scene, T>)
    T* SceneRoot::AddScene(const std::string& name, bool active, bool singleton)
    {
        if (scenes.contains(name))
        {
            SDL_Log("Scene %s already exists.", name.c_str());
            return nullptr;
        }

        std::unique_ptr<T> scene = std::make_unique<T>();
        scene->root = this;
        scene->name = name;
        scene->singleton = singleton;

        scene->InitInternal();
        if (scene->singleton)
            scene->SetActive(true);
        else
            scene->SetActive(active);

        scenes.emplace(name, std::move(scene));

        return reinterpret_cast<T*>(scenes[name].get());
    }
}