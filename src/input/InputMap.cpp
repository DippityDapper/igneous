#include "igneous/input/InputMap.hpp"

namespace Engine
{
    InputMap::InputMap(std::string id, std::string displayName)
        : id(std::move(id))
        , displayName(std::move(displayName))
    {
    }

    const std::string& InputMap::GetId() const
    {
        return id;
    }

    const std::string& InputMap::GetName() const
    {
        return displayName;
    }

    void InputMap::SetId(const std::string& value)
    {
        id = value;
    }

    void InputMap::SetName(const std::string& value)
    {
        displayName = value;
    }

    void InputMap::EnsureCache() const
    {
        if (cacheValid)
            return;

        actionCache.clear();
        for (const std::unique_ptr<InputAction>& action: actions)
        {
            if (!action || action->GetName().empty())
                continue;
            actionCache[action->GetName()] = action.get();
        }
        cacheValid = true;
    }

    void InputMap::InvalidateCache()
    {
        cacheValid = false;
    }

    InputAction* InputMap::FindAction(const std::string& name)
    {
        EnsureCache();
        auto it = actionCache.find(name);
        if (it == actionCache.end())
            return nullptr;
        return it->second;
    }

    const InputAction* InputMap::FindAction(const std::string& name) const
    {
        EnsureCache();
        auto it = actionCache.find(name);
        if (it == actionCache.end())
            return nullptr;
        return it->second;
    }

    InputAction& InputMap::AddAction(const std::string& name)
    {
        if (InputAction* existing = FindAction(name))
            return *existing;

        actions.push_back(std::make_unique<InputAction>(name));
        InvalidateCache();
        return *actions.back();
    }

    bool InputMap::RemoveAction(const std::string& name)
    {
        for (auto it = actions.begin(); it != actions.end(); ++it)
        {
            if ((*it)->GetName() != name)
                continue;
            actions.erase(it);
            InvalidateCache();
            return true;
        }
        return false;
    }

    const std::vector<std::unique_ptr<InputAction>>& InputMap::GetActions() const
    {
        return actions;
    }

    void InputMap::AddKeyBinding(const std::string& actionName, SDL_Keycode key)
    {
        AddAction(actionName).AddBinding(MakeKeyBinding(key));
    }

    void InputMap::AddMouseBinding(const std::string& actionName, SDL_MouseButtonFlags button)
    {
        AddAction(actionName).AddBinding(MakeMouseButtonBinding(button));
    }

    void InputMap::AddGamepadButtonBinding(const std::string& actionName, SDL_GamepadButton button)
    {
        AddAction(actionName).AddBinding(MakeGamepadButtonBinding(button));
    }

    void InputMap::AddGamepadAxisBinding(const std::string& actionName, GamepadAxis axis)
    {
        AddAction(actionName).AddBinding(MakeGamepadAxisBinding(axis));
    }

    float InputMap::GetValue(const std::string& name) const
    {
        const InputAction* action = FindAction(name);
        return action ? action->GetValue() : 0.0f;
    }

    bool InputMap::IsPressed(const std::string& name) const
    {
        const InputAction* action = FindAction(name);
        return action ? action->IsPressed() : false;
    }

    bool InputMap::IsJustPressed(const std::string& name) const
    {
        const InputAction* action = FindAction(name);
        return action ? action->IsJustPressed() : false;
    }

    bool InputMap::IsJustReleased(const std::string& name) const
    {
        const InputAction* action = FindAction(name);
        return action ? action->IsJustReleased() : false;
    }
}
