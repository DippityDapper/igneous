#pragma once

#include <cstdint>
#include <vector>

#include "igneous/networking/NetworkEvents.hpp"

namespace Engine
{

    class ClientConnectionWatchdog
    {
      public:
        static inline const std::vector<uint8_t> PingPacket = {static_cast<uint8_t>(NetworkEventType::Ping)};

        double timeoutSeconds = 30.0;
        double pingIntervalSeconds = 1.0;
        bool enabled = false;

        void MarkActivity(double now)
        {
            lastActivity = now;
        }

        void Reset(double now)
        {
            lastActivity = now;
            lastPingSend = 0.0;
        }

        bool HasTimedOut(double now) const
        {
            return enabled && now - lastActivity >= timeoutSeconds;
        }

        bool TryConsumePingSend(double now)
        {
            if (!enabled || now - lastPingSend < pingIntervalSeconds)
                return false;

            lastPingSend = now;
            return true;
        }

      private:
        double lastActivity = 0.0;
        double lastPingSend = 0.0;
    };
}
