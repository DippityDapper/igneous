#pragma once

#include <string>
#include <vector>

#include "igneous/networking/IdentityProvider.hpp"

#ifdef IGNEOUS_STEAM_ENABLED
#include <steam/steam_api.h>
#endif

namespace Engine
{

    class SteamIdentity : public IdentityProvider
    {
#ifdef IGNEOUS_STEAM_ENABLED
      private:
        uint64_t _steamId = 0;
        std::string _steamUsername;
        HAuthTicket _ticketHandle = k_HAuthTicketInvalid;

        STEAM_GAMESERVER_CALLBACK(SteamIdentity, OnTicketValidated, ValidateAuthTicketResponse_t);
#endif

      public:
        ~SteamIdentity() override;

        uint64_t GetLocalId() override;

        std::string GetLocalUsername() override;

        void GenerateLocalId() override;

        void GenerateLocalUsername() override;

        std::vector<uint8_t> GetAuthToken(uint64_t remoteId) override;

        bool ValidateToken(uint64_t claimedId, const std::vector<uint8_t>& token) override;

        void OnDisconnect(uint64_t id) override;

        void Clean() override;
    };
}
