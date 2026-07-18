#pragma once

#include "igneous/input/InputBinding.hpp"

#include <memory>
#include <string>
#include <vector>

namespace Engine
{

    class InputAction
    {
      public:

        explicit InputAction(std::string actionName);

        const std::string& GetName() const;

        float GetValue() const;

        bool IsPressed() const;

        bool IsJustPressed() const;

        bool IsJustReleased() const;

        void AddBinding(std::unique_ptr<InputBinding> binding);

        const std::vector<std::unique_ptr<InputBinding>>& GetBindings() const;

      private:

        std::string actionName{};

        std::vector<std::unique_ptr<InputBinding>> bindings{};
    };

}
