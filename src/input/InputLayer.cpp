#include "igneous/input/InputLayer.hpp"

namespace Engine
{
    InputLayer::InputLayer(const std::string& _name, int _priority)
    {
        name = _name;
        priority = _priority;
    }

    bool InputLayer::Is(const std::string &layerName)
    {
        if (layerName == name)
        {
            return true;
        }
        return false;
    }
}
