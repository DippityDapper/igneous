#pragma once

#include "igneous/Vec2.hpp"

union SDL_Event;

namespace Engine
{
    /// Engine base camera.
    /// The camera's center is considered (0,0) in world space.
    class Camera
    {
    private:
        Vec2<float> previousPosition{0,0};
        float previousZoom = 1.0f;
        Vec2<float> mouseScreenPosition{0,0};

    public:
        /// The main camera used for rendering.
        static Camera* main;

        Vec2<float> position{0,0};
        float zoom = 1.0f;

        float minZoom = 0.1f;
        float maxZoom = 10.0f;

        /// Whether the camera should clamp its position and zoom to defined bounds.
        bool limitBounds = false;

        float limitLeft = 0;
        float limitRight = 0;
        float limitTop = 0;
        float limitBottom = 0;

    public:
        /// Constructs a camera. If no main camera exists, this instance becomes the main camera.
        explicit Camera();

        /// Constructs a camera. If no main camera exists, this instance becomes the main camera.
        /// @param x The initial x position of the camera.
        /// @param y The initial y position of the camera.
        /// @param _zoom The initial zoom of the camera.
        Camera(float x, float y, float _zoom);

        ~Camera();
        Camera(const Camera& other) = delete;

        /// Internal engine call.
        /// @note Do not call this directly unless you are extending the engine core.
        void UpdateInternal(float delta);

        /// Internal engine call.
        /// @note Do not call this directly unless you are extending the engine core.
        void HandleEventsInternal(SDL_Event& event);

        /// A virtual function that is called every engine loop.
        /// @param delta delta time for the current iteration.
        virtual void Update(float delta){};

        /// A virtual function that is called every engine loop for each SDL event that has occurred this iteration.
        /// @param event The event object.
        virtual void HandleEvents(SDL_Event& event){};

        /// Set this camera as the main camera.
        void SetCurrent();

        /// Ensures the camera's zoom and position stay within the defined bounds.
        /// Called automatically during the internal update loop if `limitBounds` is true.
        void BoundsCheck();

        /// Checks if a position is within the bounds of the set limits.
        /// @param _position The position to check.
        /// @returns true if the position is within the camera bounds, false otherwise.
        bool IsWithinBounds(Vec2<float> _position);

        /// Clamps the camera position to be within the defined bounds using the current zoom level.
        /// @param targetPosition The position to clamp.
        /// @returns The clamped position.
        Vec2<float> ClampToBounds(Vec2<float> targetPosition);

        /// Clamps the camera position to defined bounds given the target zoom.
        /// @param targetPosition The position to clamp.
        /// @param targetZoom The zoom level to use when clamping.
        /// @returns The clamped position.
        Vec2<float> ClampToBounds(Vec2<float> targetPosition, float targetZoom);

        /// Clamps the zoom of the camera to the defined bounds.
        /// @param targetZoom The zoom level to clamp.
        /// @returns The clamped zoom.
        float ClampToBounds(float targetZoom);

        Vec2<float> GetMouseScreenPosition();

        Vec2<float> GetMouseGlobalPosition();
    };
}
