#pragma once

namespace Engine
{

    enum class GamepadAxis
    {
        LeftXRight,
        LeftXLeft,
        LeftYUp,
        LeftYDown,
        RightXRight,
        RightXLeft,
        RightYUp,
        RightYDown,
        TriggerLeft,
        TriggerRight,
    };

    const char* GamepadAxisToString(GamepadAxis axis);

    bool GamepadAxisFromString(const char* name, GamepadAxis& outAxis);

}
