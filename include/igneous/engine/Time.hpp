#pragma once

#include <cstdint>

namespace Engine
{
    class Time
    {
    public:
        static inline uint64_t lastTick = 0;
        static inline uint64_t currentTick = 0;
        static inline float deltaTime = 0;
    };
}
