#include "igneous/input/InputBinding.hpp"

#include "igneous/input/Input.hpp"

namespace Engine
{
    KeyBinding::KeyBinding(SDL_Keycode key)
        : key(key)
    {
    }

    InputBindingType KeyBinding::GetType() const
    {
        return InputBindingType::Key;
    }

    float KeyBinding::GetValue() const
    {
        return Input::IsKeyDown(key) ? 1.0f : 0.0f;
    }

    bool KeyBinding::IsJustPressed() const
    {
        return Input::IsKeyJustPressed(key);
    }

    bool KeyBinding::IsJustReleased() const
    {
        return Input::IsKeyJustReleased(key);
    }

    SDL_Keycode KeyBinding::GetKey() const
    {
        return key;
    }

    MouseButtonBinding::MouseButtonBinding(SDL_MouseButtonFlags button)
        : button(button)
    {
    }

    InputBindingType MouseButtonBinding::GetType() const
    {
        return InputBindingType::Mouse;
    }

    float MouseButtonBinding::GetValue() const
    {
        return Input::IsMouseButtonDown(button) ? 1.0f : 0.0f;
    }

    bool MouseButtonBinding::IsJustPressed() const
    {
        return Input::IsButtonJustPressed(button);
    }

    bool MouseButtonBinding::IsJustReleased() const
    {
        return Input::IsButtonJustReleased(button);
    }

    SDL_MouseButtonFlags MouseButtonBinding::GetButton() const
    {
        return button;
    }

    GamepadButtonBinding::GamepadButtonBinding(SDL_GamepadButton button)
        : button(button)
    {
    }

    InputBindingType GamepadButtonBinding::GetType() const
    {
        return InputBindingType::GamepadButton;
    }

    float GamepadButtonBinding::GetValue() const
    {
        for (SDL_JoystickID deviceId: Input::GetConnectedGamepads())
        {
            if (Input::IsGamepadButtonDown(deviceId, button))
                return 1.0f;
        }
        return 0.0f;
    }

    bool GamepadButtonBinding::IsJustPressed() const
    {
        for (SDL_JoystickID deviceId: Input::GetConnectedGamepads())
        {
            if (Input::IsGamepadButtonJustPressed(deviceId, button))
                return true;
        }
        return false;
    }

    bool GamepadButtonBinding::IsJustReleased() const
    {
        for (SDL_JoystickID deviceId: Input::GetConnectedGamepads())
        {
            if (Input::IsGamepadButtonJustReleased(deviceId, button))
                return true;
        }
        return false;
    }

    SDL_GamepadButton GamepadButtonBinding::GetButton() const
    {
        return button;
    }

    GamepadAxisBinding::GamepadAxisBinding(GamepadAxis axis)
        : axis(axis)
    {
    }

    InputBindingType GamepadAxisBinding::GetType() const
    {
        return InputBindingType::GamepadAxis;
    }

    float GamepadAxisBinding::GetValue() const
    {
        for (SDL_JoystickID deviceId: Input::GetConnectedGamepads())
        {
            float value = Input::GetSemanticGamepadAxis(deviceId, axis);
            if (value != 0.0f)
                return value;
        }
        return 0.0f;
    }

    bool GamepadAxisBinding::IsJustPressed() const
    {
        return false;
    }

    bool GamepadAxisBinding::IsJustReleased() const
    {
        return false;
    }

    GamepadAxis GamepadAxisBinding::GetAxis() const
    {
        return axis;
    }

    std::unique_ptr<InputBinding> MakeKeyBinding(SDL_Keycode key)
    {
        return std::make_unique<KeyBinding>(key);
    }

    std::unique_ptr<InputBinding> MakeMouseButtonBinding(SDL_MouseButtonFlags button)
    {
        return std::make_unique<MouseButtonBinding>(button);
    }

    std::unique_ptr<InputBinding> MakeGamepadButtonBinding(SDL_GamepadButton button)
    {
        return std::make_unique<GamepadButtonBinding>(button);
    }

    std::unique_ptr<InputBinding> MakeGamepadAxisBinding(GamepadAxis axis)
    {
        return std::make_unique<GamepadAxisBinding>(axis);
    }
}
