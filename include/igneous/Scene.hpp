#pragma once

#include <cstdint>
#include <string>

union SDL_Event;

namespace Engine
{
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
        uint32_t id = 0;
        std::string name{};

        bool active = false;
        bool singleton = false;

    public:
        virtual ~Scene() = default;

        /// Initializes the scene.
        /// @internal
        /// @note This is called internally within the Engine.
        void InitInternal();

        /// Updates the scene.
        /// @internal
        /// @note This is called internally within the Engine.
        void UpdateInternal(float delta);

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
        void HandleEventsInternal(SDL_Event& event);

    private:
        /// A virtual function that is called on initialization of the scene.
        virtual void Init(){};

        /// A virtual function called every Engine loop.
        /// @param delta delta time.
        /// @note Rendering textures should be done in the Rendering() call.
        virtual void Update(float delta){};

        /// A virtual function for rendering scene components.
        virtual void Render(){};

        /// A virtual function that handles events.
        /// @param event The event being handled.
        virtual void HandleEvents(SDL_Event& event){};

        /// A virtual function that cleans a scene and all of it's components.
        virtual void Clean(){};

    public:
        // void Init() override;
        // void Update(float delta) override;
        // void Render() override;
        // void HandleEvents(SDL_Event& event) override;
        // void Clean() override;
    };
}
