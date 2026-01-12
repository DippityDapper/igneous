#pragma once

#include <string>

#include "igneous/input/InputEvent.hpp"
#include "igneous/input/InputLayer.hpp"

namespace Engine
{
    class SceneRoot;

    /// Engine base scene.
    ///
    /// The logic flow looks like this:
    /// @code
    /// Init() // Upon setting the scene as the current scene
    /// HandleEvents()
    /// Update()
    /// Render()
    /// Clean() // When a new scene is set, the old scene is cleaned
    /// @endcode

    class Scene
    {
    public:
        SceneRoot* root = nullptr;
        std::string name{};
        bool singleton = false;

    private:
        bool active = false;

    public:
        virtual ~Scene();

        /// Initializes the scene.
        /// @internal
        /// @note This is called internally within the Engine.
        void InitInternal();

        /// Updates the scene.
        /// @internal
        /// @note This is called internally within the Engine.
        void UpdateInternal(float delta);

        void UIInternal(InputLayer& layer);

        /// Renders the scene.
        /// @internal
        /// @note This is called internally within the Engine.
        void RenderInternal();

        /// Cleans the scene.
        /// @internal
        /// @note This is called internally within the Engine.
        void CleanInternal();

        /// Handles events in the scene.
        /// @internal
        /// @note This is called internally within the Engine.
        void HandleEventsInternal(InputLayer& layer);

        void SetActive(bool value);

        bool IsActive();

    private:
        /// A virtual function that is called on initialization of the scene.
        virtual void Init(){}

        /// A virtual function called every Engine loop.
        /// @param delta delta time.
        /// @note Rendering textures should be done in the Rendering() call.
        virtual void Update(float delta){}

        virtual void UI(InputLayer& layer){}

        /// A virtual function for rendering scene components.
        virtual void Render(){}

        /// A virtual function that handles events.
        /// @param event The event being handled.
        virtual void HandleEvents(InputLayer& layer){}

        /// A virtual function that cleans a scene and all of it's components.
        virtual void Clean(){}

        virtual void OnActiveChanged(bool value){}
    };
}
