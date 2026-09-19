#pragma once

#include "igneous/input/Input.hpp"
#include "igneous/input/InputMap.hpp"

namespace Engine
{

    class InputMapQuery
    {
      public:

        static float GetAxisFromValues(float positiveValue, float negativeValue)
        {
            return positiveValue - negativeValue;
        }

        static float GetAxis(const std::string& positiveAction, const std::string& negativeAction, const InputMap& inputMap)
        {
            return GetAxisFromValues(inputMap.GetValue(positiveAction), inputMap.GetValue(negativeAction));
        }

        static float GetAxis(const std::string& positiveAction, const std::string& negativeAction)
        {
            InputMap* inputMap = Input::GetInputMap();
            if (!inputMap)
                return 0.0f;
            return GetAxis(positiveAction, negativeAction, *inputMap);
        }
    };

}
