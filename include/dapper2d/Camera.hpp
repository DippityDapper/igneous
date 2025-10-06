#pragma once

#include "dapper2d/Vec2.hpp"

union SDL_Event;

namespace Engine
{
    class Camera
    {
    public:
        static Camera* main;

        Vec2<float> position{0,0};
        float zoom = 1.0f;

        Vec2<float> previousPosition{0,0};
        float previousZoom = 1.0f;

        bool panning = false;

        bool limitBounds = false;

        float limitLeft = 0;
        float limitRight = 0;
        float limitTop = 0;
        float limitBottom = 0;

    public:
        explicit Camera();
        Camera(float x, float y, float _zoom);
        ~Camera();

        void UpdateInternal(float delta);
        void HandleEventsInternal(SDL_Event& event);

        virtual void Update(float delta){};
        virtual void HandleEvents(SDL_Event& event){};

        void SetCurrent();
        void BoundsCheck();
        bool IsWithinBounds(Vec2<float> _position);
        Vec2<float> ClampToBounds(Vec2<float> targetPosition);
        float ClampToBounds(float targetZoom);
        Vec2<float> ClampToBounds(Vec2<float> targetPosition, float targetZoom);
    };
}
