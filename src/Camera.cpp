#include "dapper2d/Camera.hpp"

#include "SDL3/SDL.h"

namespace Engine
{
    Camera* Camera::main = nullptr;

    Camera::Camera()
    {
        if (main == nullptr)
            main = this;
    }

    Camera::Camera(float x, float y, float _zoom)
    {
        if (main == nullptr)
            main = this;

        position.x = x;
        position.y = y;
        zoom = _zoom;
    }

    Camera::~Camera()
    {
        if (main == this)
            main = nullptr;
    }

    void Camera::Update(float delta)
    {

    }

    void Camera::HandleEvents(SDL_Event& event)
    {
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            if (event.button.button == SDL_BUTTON_MIDDLE)
            {
                panning = true;
            }
        }
        if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            if (event.button.button == SDL_BUTTON_MIDDLE)
            {
                panning = false;
            }
        }
        if (event.type == SDL_EVENT_MOUSE_MOTION && panning)
        {
            position.x -= event.motion.xrel / zoom;
            position.y -= event.motion.yrel / zoom;
        }
        if (event.type == SDL_EVENT_MOUSE_WHEEL)
        {
            if (event.wheel.y > 0)
                zoom *= 1.1f;
            else if (event.wheel.y < 0)
                zoom *= 0.9f;

            if (zoom < minZoom)
                zoom = 0.1f;
            if (zoom > maxZoom)
                zoom = 10.0f;
        }
    }

    void Camera::SetCurrent()
    {
        main = this;
    }
}
