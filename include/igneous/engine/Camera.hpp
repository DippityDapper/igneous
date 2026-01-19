#pragma once

#include "igneous/engine/Vec2.hpp"
#include "igneous/input/InputEvent.hpp"
#include "igneous/input/InputLayer.hpp"

union SDL_Event;

namespace Engine
{
    /**
     * @class Camera
     * @brief Engine base camera for viewport control and world-space positioning.
     *
     * Camera manages the rendering viewport's position and zoom level in world space.
     * The camera's center is considered (0,0) in world space. It supports optional
     * boundary constraints to limit the viewable area and provides utilities for
     * converting between screen and world coordinates.
     *
     * The Camera class follows a singleton-like pattern where one camera can be
     * designated as the "main" camera used for rendering.
     *
     * @note The camera's position represents its center in world space.
     * @note Boundary checking is only performed when limitBounds is true.
     */
    class Camera
    {
      private:
        /**
         * @brief Previous frame's camera position for change detection.
         */
        Vec2<float> previousPosition{0, 0};

        /**
         * @brief Previous frame's zoom level for change detection.
         */
        float previousZoom = 1.0f;

      public:
        /**
         * @brief The main camera used for rendering.
         *
         * This static pointer references the primary camera instance used by
         * the rendering system. If null, no main camera is set.
         */
        static Camera* main;

        /**
         * @brief Current position of the camera's center in world space.
         */
        Vec2<float> position{0, 0};

        /**
         * @brief Current zoom level of the camera.
         *
         * Values greater than 1.0 zoom in, values less than 1.0 zoom out.
         */
        float zoom = 1.0f;

        /**
         * @brief Minimum allowed zoom level.
         */
        float minZoom = 0.1f;

        /**
         * @brief Maximum allowed zoom level.
         */
        float maxZoom = 10.0f;

        /**
         * @brief Whether the camera should clamp its position and zoom to defined bounds.
         *
         * When true, the camera's position and zoom are automatically constrained
         * to stay within the defined limit boundaries.
         */
        bool limitBounds = false;

        /**
         * @brief Left boundary limit in world space.
         */
        float limitLeft = 0;

        /**
         * @brief Right boundary limit in world space.
         */
        float limitRight = 0;

        /**
         * @brief Top boundary limit in world space.
         */
        float limitTop = 0;

        /**
         * @brief Bottom boundary limit in world space.
         */
        float limitBottom = 0;

      public:
        /**
         * @brief Constructs a camera.
         *
         * If no main camera exists, this instance becomes the main camera.
         */
        explicit Camera();

        /**
         * @brief Constructs a camera with initial position and zoom.
         *
         * If no main camera exists, this instance becomes the main camera.
         *
         * @param x Initial x position of the camera center.
         * @param y Initial y position of the camera center.
         * @param _zoom Initial zoom level.
         */
        Camera(float x, float y, float _zoom);

        /**
         * @brief Destructor.
         *
         * If this camera is the main camera, sets the main camera pointer to null.
         */
        ~Camera();

        /**
         * @brief Deleted copy constructor.
         */
        Camera(const Camera& other) = delete;

        /**
         * @brief Internal engine update call.
         *
         * Performs bounds checking if enabled and position/zoom have changed,
         * then calls the virtual Update() method.
         *
         * @param delta Delta time for the current frame.
         *
         * @note Do not call this directly unless you are extending the engine core.
         */
        void UpdateInternal(float delta);

        /**
         * @brief Internal engine event handling call.
         *
         * Handles window resize events and performs bounds checking if needed,
         * then calls the virtual HandleEvents() method.
         *
         * @param layer Input layer containing event data.
         *
         * @note Do not call this directly unless you are extending the engine core.
         */
        void HandleEventsInternal(InputLayer& layer);

        /**
         * @brief Virtual update function called every engine loop.
         *
         * Override this method to implement custom camera behavior that runs
         * each frame.
         *
         * @param delta Delta time for the current frame.
         */
        virtual void Update(float delta) {};

        /**
         * @brief Virtual event handling function called every engine loop.
         *
         * Override this method to implement custom camera input handling.
         *
         * @param layer Input layer containing event data.
         */
        virtual void HandleEvents(InputLayer& layer) {};

        /**
         * @brief Sets this camera as the main camera.
         *
         * Updates the static main pointer to reference this camera instance.
         */
        void SetCurrent();

        /**
         * @brief Ensures the camera's zoom and position stay within defined bounds.
         *
         * Clamps both zoom and position to their respective limits. Called
         * automatically during the internal update loop if limitBounds is true.
         */
        void BoundsCheck();

        /**
         * @brief Checks if a position is within the camera's boundary limits.
         *
         * @param _position The position to check in world space.
         * @return true if the position is within bounds, false otherwise.
         */
        bool IsWithinBounds(Vec2<float> _position);

        /**
         * @brief Clamps a camera position to defined bounds using the current zoom.
         *
         * @param targetPosition The position to clamp.
         * @return The clamped position.
         */
        Vec2<float> ClampToBounds(Vec2<float> targetPosition);

        /**
         * @brief Clamps a camera position to defined bounds using a target zoom.
         *
         * @param targetPosition The position to clamp.
         * @param targetZoom The zoom level to use when calculating bounds.
         * @return The clamped position.
         */
        Vec2<float> ClampToBounds(Vec2<float> targetPosition, float targetZoom);

        /**
         * @brief Clamps a zoom level to the defined bounds.
         *
         * Ensures the zoom level doesn't exceed min/max zoom limits and doesn't
         * allow zooming out further than the world boundaries.
         *
         * @param targetZoom The zoom level to clamp.
         * @return The clamped zoom level.
         */
        float ClampToBounds(float targetZoom);

        /**
         * @brief Converts the mouse screen position to world space coordinates.
         *
         * Takes the current mouse position on screen and transforms it to the
         * corresponding position in world space using the camera's position and zoom.
         *
         * @return The mouse position in world space.
         */
        Vec2<float> GetMouseGlobalPosition();
    };
}