#pragma once

#include <string>

namespace Engine
{
    class InputLayer
    {
    private:
        std::string name{};
        int priority = 0;

    public:
        InputLayer(const std::string& _name, int _priority);

        bool Is(const std::string& layerName);
    };
}
