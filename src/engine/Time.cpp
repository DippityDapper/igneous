#include "igneous/engine/Time.hpp"

namespace Engine
{
    void Time::ResetForTests()
    {
        lastTick = 0;
        currentTick = 0;
        deltaTime = 0;
    }
}
