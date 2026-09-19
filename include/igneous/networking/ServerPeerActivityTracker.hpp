#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace Engine
{

    class ServerPeerActivityTracker
    {
      public:
        double timeoutSeconds = 10.0;
        double pingIntervalSeconds = 1.0;

        void TrackPeer(uint32_t peerId, double now)
        {
            lastActivity[peerId] = now;
        }

        void MarkActivity(uint32_t peerId, double now)
        {
            lastActivity[peerId] = now;
        }

        bool RemovePeer(uint32_t peerId)
        {
            return lastActivity.erase(peerId) > 0;
        }

        void Clear()
        {
            lastActivity.clear();
        }

        bool TryConsumePingSend(double now)
        {
            if (now - lastPingSend < pingIntervalSeconds)
                return false;

            lastPingSend = now;
            return true;
        }

        std::vector<uint32_t> CollectStalePeerIds(double now) const
        {
            std::vector<uint32_t> stalePeerIds;
            for (const auto& [peerId, activity]: lastActivity)
            {
                if (now - activity >= timeoutSeconds)
                    stalePeerIds.push_back(peerId);
            }
            return stalePeerIds;
        }

      private:
        std::unordered_map<uint32_t, double> lastActivity{};
        double lastPingSend = 0.0;
    };
}
