#include "igneous/input/InputLayer.hpp"

namespace Engine
{
    InputLayer::InputLayer(const std::string& _name, int _priority)
    {
        name = _name;
        priority = _priority;
    }

    bool InputLayer::Is(const std::string& layerName)
    {
        return layerName == name;
    }

    const std::string& InputLayer::GetName() const
    {
        return name;
    }

    int InputLayer::GetPriority() const
    {
        return priority;
    }
}
