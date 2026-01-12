#pragma once

namespace Engine
{
    struct InputEvent
    {
        bool pressed = false;
        bool pressedLastFrame = false;
        bool handled = false;
    };
}
