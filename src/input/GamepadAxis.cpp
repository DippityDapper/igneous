#include "igneous/input/GamepadAxis.hpp"

#include <cstring>

namespace Engine
{
    const char* GamepadAxisToString(GamepadAxis axis)
    {
        switch (axis)
        {
            case GamepadAxis::LeftXRight:
                return "left_x_right";
            case GamepadAxis::LeftXLeft:
                return "left_x_left";
            case GamepadAxis::LeftYUp:
                return "left_y_up";
            case GamepadAxis::LeftYDown:
                return "left_y_down";
            case GamepadAxis::RightXRight:
                return "right_x_right";
            case GamepadAxis::RightXLeft:
                return "right_x_left";
            case GamepadAxis::RightYUp:
                return "right_y_up";
            case GamepadAxis::RightYDown:
                return "right_y_down";
            case GamepadAxis::TriggerLeft:
                return "trigger_left";
            case GamepadAxis::TriggerRight:
                return "trigger_right";
        }
        return "left_x_left";
    }

    bool GamepadAxisFromString(const char* name, GamepadAxis& outAxis)
    {
        if (!name)
            return false;

        struct AxisName
        {
            const char* name;
            GamepadAxis axis;
        };

        static constexpr AxisName kAxisNames[] = {
            {"left_x_right", GamepadAxis::LeftXRight},
            {"left_x_left", GamepadAxis::LeftXLeft},
            {"left_y_up", GamepadAxis::LeftYUp},
            {"left_y_down", GamepadAxis::LeftYDown},
            {"right_x_right", GamepadAxis::RightXRight},
            {"right_x_left", GamepadAxis::RightXLeft},
            {"right_y_up", GamepadAxis::RightYUp},
            {"right_y_down", GamepadAxis::RightYDown},
            {"trigger_left", GamepadAxis::TriggerLeft},
            {"trigger_right", GamepadAxis::TriggerRight},
        };

        for (const AxisName& entry: kAxisNames)
        {
            if (std::strcmp(name, entry.name) == 0)
            {
                outAxis = entry.axis;
                return true;
            }
        }

        return false;
    }
}
