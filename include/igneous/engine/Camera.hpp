// Doc: docs/classes/Camera.md
#pragma once

#include "igneous/engine/Vec2.hpp"
#include "igneous/input/InputEvent.hpp"
#include "igneous/input/InputLayer.hpp"

union SDL_Event;

namespace Engine
{

    class Camera
    {
      private:

        Vec2<float> previousPosition{0, 0};

        float previousZoom = 1.0f;

      public:

        static Camera* main;

        Vec2<float> position{0, 0};

        float zoom = 1.0f;

        float minZoom = 0.1f;

        float maxZoom = 10.0f;

        bool limitBounds = false;

        float limitLeft = 0;

        float limitRight = 0;

        float limitTop = 0;

        float limitBottom = 0;

      public:

        /// Clears the active main camera pointer between unit tests.
        static void ResetForTests();

        explicit Camera();

        Camera(float x, float y, float _zoom);

        virtual ~Camera();

        Camera(const Camera& other) = delete;

        void UpdateInternal(double delta);

        void HandleEventsInternal(InputLayer& layer);

        virtual void Update(double delta) {};

        virtual void HandleInputs(InputLayer& layer) {};

        void SetCurrent();

        void BoundsCheck();

        bool IsWithinBounds(Vec2<float> _position);

        Vec2<float> ClampToBounds(Vec2<float> targetPosition);

        Vec2<float> ClampToBounds(Vec2<float> targetPosition, float targetZoom);

        float ClampToBounds(float targetZoom);

        Vec2<float> GetMouseGlobalPosition();
    };
}
