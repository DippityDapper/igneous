#include "igneous/input/InputAction.hpp"

namespace Engine
{
    InputAction::InputAction(std::string actionName)
        : actionName(std::move(actionName))
    {
    }

    const std::string& InputAction::GetName() const
    {
        return actionName;
    }

    float InputAction::GetValue() const
    {
        for (const std::unique_ptr<InputBinding>& binding: bindings)
        {
            float value = binding->GetValue();
            if (value != 0.0f)
                return value;
        }
        return 0.0f;
    }

    bool InputAction::IsPressed() const
    {
        return GetValue() != 0.0f;
    }

    bool InputAction::IsJustPressed() const
    {
        for (const std::unique_ptr<InputBinding>& binding: bindings)
        {
            if (binding->IsJustPressed())
                return true;
        }
        return false;
    }

    bool InputAction::IsJustReleased() const
    {
        for (const std::unique_ptr<InputBinding>& binding: bindings)
        {
            if (binding->IsJustReleased())
                return true;
        }
        return false;
    }

    void InputAction::AddBinding(std::unique_ptr<InputBinding> binding)
    {
        bindings.push_back(std::move(binding));
    }

    const std::vector<std::unique_ptr<InputBinding>>& InputAction::GetBindings() const
    {
        return bindings;
    }
}
