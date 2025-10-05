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
        bool panning = false;
        float minZoom = 0.1f;
        float maxZoom = 10.0f;

    public:
        explicit Camera();
        Camera(float x, float y, float _zoom);
        ~Camera();

        void Update(float delta);
        void HandleEvents(SDL_Event& event);
        void SetCurrent();
    };
}
