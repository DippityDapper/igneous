// Doc: docs/classes/InputMap.md
#pragma once

#include "igneous/input/InputAction.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Engine
{

    class InputMap
    {
      public:

        InputMap() = default;

        InputMap(std::string id, std::string displayName);

        const std::string& GetId() const;

        const std::string& GetName() const;

        void SetId(const std::string& value);

        void SetName(const std::string& value);

        InputAction* FindAction(const std::string& name);

        const InputAction* FindAction(const std::string& name) const;

        InputAction& AddAction(const std::string& name);

        bool RemoveAction(const std::string& name);

        const std::vector<std::unique_ptr<InputAction>>& GetActions() const;

        void AddKeyBinding(const std::string& actionName, SDL_Keycode key);

        void AddMouseBinding(const std::string& actionName, SDL_MouseButtonFlags button);

        void AddGamepadButtonBinding(const std::string& actionName, SDL_GamepadButton button);

        void AddGamepadAxisBinding(const std::string& actionName, GamepadAxis axis);

        float GetValue(const std::string& name) const;

        bool IsPressed(const std::string& name) const;

        bool IsJustPressed(const std::string& name) const;

        bool IsJustReleased(const std::string& name) const;

      private:

        void EnsureCache() const;

        void InvalidateCache();

        std::string id{};

        std::string displayName{};

        std::vector<std::unique_ptr<InputAction>> actions{};

        mutable std::unordered_map<std::string, InputAction*> actionCache{};

        mutable bool cacheValid = false;
    };

}
