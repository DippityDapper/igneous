#include "igneous/engine/Camera.hpp"

#include "igneous/input/Input.hpp"
#include "SDL3/SDL.h"
#include "igneous/rendering/Window.hpp"

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

    void Camera::UpdateInternal(float delta)
    {
        if (limitBounds && (previousPosition != position || previousZoom != zoom))
        {
            BoundsCheck();
            previousPosition = position;
            previousZoom = zoom;
        }
        Update(delta);
    }

    void Camera::HandleEventsInternal(InputLayer& layer)
    {
        if (Input::IsWindowResized())
        {
            if (limitBounds)
            {
                BoundsCheck();
            }
        }
        HandleEvents(layer);
    }

    bool Camera::IsWithinBounds(Vec2<float> _position)
    {
        Vec2<float> offset = (Vec2<float>)Window::viewport / 2.0f / zoom;
        if (_position.x < limitLeft + offset.x)
            return false;
        if (_position.x > limitRight - offset.x)
            return false;
        if (_position.y < limitTop + offset.y)
            return false;
        if (_position.y > limitBottom - offset.y)
            return false;
        return true;
    }

    Vec2<float> Camera::ClampToBounds(Vec2<float> targetPosition)
    {
        return ClampToBounds(targetPosition, zoom);
    }

    Vec2<float> Camera::ClampToBounds(Vec2<float> targetPosition, float targetZoom)
    {
        Vec2<float> newPos = targetPosition;

        Vec2<float> offset = (Vec2<float>)Window::viewport / 2.0f / targetZoom;
        if (targetPosition.x < limitLeft + offset.x)
            newPos.x = limitLeft + offset.x;
        if (targetPosition.x > limitRight - offset.x)
            newPos.x = limitRight - offset.x;
        if (targetPosition.y < limitTop + offset.y)
            newPos.y = limitTop + offset.y;
        if (targetPosition.y > limitBottom - offset.y)
            newPos.y = limitBottom - offset.y;
        return newPos;
    }

    float Camera::ClampToBounds(float targetZoom)
    {
        float worldWidth  = limitRight - limitLeft;
        float worldHeight = limitBottom - limitTop;

        float maxZoomOutX = (float)Window::viewport.x / worldWidth;
        float maxZoomOutY = (float)Window::viewport.y / worldHeight;
        float maxAllowedZoomOut = std::max(maxZoomOutX, maxZoomOutY);

        if (targetZoom < maxAllowedZoomOut)
            return maxAllowedZoomOut;
        return targetZoom;
    }

    Vec2<float> Camera::GetMouseGlobalPosition()
    {
        float viewportX = (float)Window::viewport.x;
        float viewportY = (float)Window::viewport.y;

        float screenOffsetX = viewportX / 2.0f;
        float screenOffsetY = viewportY / 2.0f;

        float mouseGlobalX = (Input::GetMouseScreenPosition().x - screenOffsetX) / zoom + position.x;
        float mouseGlobalY = (Input::GetMouseScreenPosition().y - screenOffsetY) / zoom + position.y;

        return {mouseGlobalX, mouseGlobalY};
    }

    void Camera::BoundsCheck()
    {
        zoom = ClampToBounds(zoom);
        position = ClampToBounds(position);
    }

    void Camera::SetCurrent()
    {
        main = this;
    }
}
