// Doc: docs/classes/InputBinding.md
#pragma once

#include "igneous/input/GamepadAxis.hpp"

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

#include <memory>

namespace Engine
{

    enum class InputBindingType
    {
        Key,
        Mouse,
        GamepadButton,
        GamepadAxis,
    };

    class InputBinding
    {
      public:

        virtual ~InputBinding() = default;

        virtual InputBindingType GetType() const = 0;

        virtual float GetValue() const = 0;

        virtual bool IsJustPressed() const = 0;

        virtual bool IsJustReleased() const = 0;
    };

    class KeyBinding final : public InputBinding
    {
      public:

        explicit KeyBinding(SDL_Keycode key);

        InputBindingType GetType() const override;

        float GetValue() const override;

        bool IsJustPressed() const override;

        bool IsJustReleased() const override;

        SDL_Keycode GetKey() const;

      private:

        SDL_Keycode key{};
    };

    class MouseButtonBinding final : public InputBinding
    {
      public:

        explicit MouseButtonBinding(SDL_MouseButtonFlags button);

        InputBindingType GetType() const override;

        float GetValue() const override;

        bool IsJustPressed() const override;

        bool IsJustReleased() const override;

        SDL_MouseButtonFlags GetButton() const;

      private:

        SDL_MouseButtonFlags button{};
    };

    class GamepadButtonBinding final : public InputBinding
    {
      public:

        explicit GamepadButtonBinding(SDL_GamepadButton button);

        InputBindingType GetType() const override;

        float GetValue() const override;

        bool IsJustPressed() const override;

        bool IsJustReleased() const override;

        SDL_GamepadButton GetButton() const;

      private:

        SDL_GamepadButton button{};
    };

    class GamepadAxisBinding final : public InputBinding
    {
      public:

        explicit GamepadAxisBinding(GamepadAxis axis);

        InputBindingType GetType() const override;

        float GetValue() const override;

        bool IsJustPressed() const override;

        bool IsJustReleased() const override;

        GamepadAxis GetAxis() const;

      private:

        GamepadAxis axis{};
    };

    std::unique_ptr<InputBinding> MakeKeyBinding(SDL_Keycode key);

    std::unique_ptr<InputBinding> MakeMouseButtonBinding(SDL_MouseButtonFlags button);

    std::unique_ptr<InputBinding> MakeGamepadButtonBinding(SDL_GamepadButton button);

    std::unique_ptr<InputBinding> MakeGamepadAxisBinding(GamepadAxis axis);

}
