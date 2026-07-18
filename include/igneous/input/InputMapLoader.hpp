// Doc: docs/classes/InputMapLoader.md
#pragma once

#include "igneous/input/InputMap.hpp"

#include <memory>
#include <string>

namespace Engine
{

    class InputMapLoader
    {
      public:

        static std::shared_ptr<InputMap> LoadFromFile(const std::string& path);

        static std::shared_ptr<InputMap> LoadFromString(const std::string& json);

        static bool SaveToFile(const InputMap& inputMap, const std::string& path);

        static std::string SaveToString(const InputMap& inputMap);
    };

}
