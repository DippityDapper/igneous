// Doc: docs/classes/ILoopbackNetwork.md
#pragma once

#include "igneous/networking/NetworkLoopbackLink.hpp"

namespace Engine
{

    class ILoopbackNetwork
    {
      public:
        virtual ~ILoopbackNetwork() = default;

        virtual NetworkLoopbackLink& GetLoopback() = 0;
    };
}
