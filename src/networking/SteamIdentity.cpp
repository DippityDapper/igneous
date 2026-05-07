#include "igneous/networking/SteamIdentity.hpp"

#include "steam/isteamgameserver.h"

#include <SDL3/SDL_log.h>

namespace Engine
{

    // =============================================================================
    // Steam-enabled implementation
    // =============================================================================

#ifdef IGNEOUS_STEAM_ENABLED

    SteamIdentity::~SteamIdentity()
    {
        SteamIdentity::Clean();
    }

    uint64_t SteamIdentity::GetLocalId()
    {
        return _steamId;
    }

    std::string SteamIdentity::GetLocalUsername()
    {
        return _steamUsername;
    }

    void SteamIdentity::GenerateLocalId()
    {
        _steamId = SteamUser()->GetSteamID().ConvertToUint64();
    }

    void SteamIdentity::GenerateLocalUsername()
    {
        _steamUsername = SteamFriends()->GetPersonaName();
    }

    std::vector<uint8_t> SteamIdentity::GetAuthToken(uint64_t remoteId)
    {
        // Bind the ticket to the specific server's SteamID to prevent replay attacks
        SteamNetworkingIdentity remoteIdentity{};
        uint64 id = remoteId;
        remoteIdentity.SetSteamID(CSteamID(id));

        uint8_t buffer[1024];
        uint32_t length = 0;

        _ticketHandle = SteamUser()->GetAuthSessionTicket(buffer, sizeof(buffer), &length, &remoteIdentity);

        if (_ticketHandle == k_HAuthTicketInvalid || length == 0)
        {
            SDL_Log("SteamIdentity: Failed to generate auth session ticket.");
            return {};
        }

        return std::vector<uint8_t>(buffer, buffer + length);
    }

    bool SteamIdentity::ValidateToken(uint64_t claimedId, const std::vector<uint8_t>& token)
    {
        uint64 id = claimedId;
        EBeginAuthSessionResult result = SteamGameServer()->BeginAuthSession(
                token.data(),
                static_cast<int>(token.size()),
                CSteamID(id));

        if (result != k_EBeginAuthSessionResultOK)
        {
            SDL_Log("SteamIdentity: BeginAuthSession rejected immediately (result=%d).", static_cast<int>(result));

            if (OnAuthResult)
                OnAuthResult(claimedId, false);

            return false;
        }

        // Validation is async — OnTicketValidated fires the OnAuthResult callback
        return true;
    }

    void SteamIdentity::OnDisconnect(uint64_t id)
    {
        uint64 _id = id;
        SteamGameServer()->EndAuthSession(CSteamID(_id));
    }

    void SteamIdentity::OnTicketValidated(ValidateAuthTicketResponse_t* pResponse)
    {
        bool valid = pResponse->m_eAuthSessionResponse == k_EAuthSessionResponseOK;

        if (OnAuthResult)
            OnAuthResult(pResponse->m_SteamID.ConvertToUint64(), valid);
    }

    void SteamIdentity::Clean()
    {
        if (_ticketHandle != k_HAuthTicketInvalid)
        {
            SteamUser()->CancelAuthTicket(_ticketHandle);
            _ticketHandle = k_HAuthTicketInvalid;
        }
    }

    // =============================================================================
    // Stub implementation — Steamworks not available
    // =============================================================================

#else

    SteamIdentity::SteamIdentity()
    {
        SDL_Log("SteamIdentity: Steamworks support was not compiled into this build of Igneous. "
                "Enable it with -DIGNEOUS_STEAM=ON and provide the Steamworks SDK.");
    }

    SteamIdentity::~SteamIdentity() = default;

    uint64_t SteamIdentity::GetLocalId()
    {
        return 0;
    }
    std::string SteamIdentity::GetLocalUsername()
    {
        return {};
    }
    void SteamIdentity::GenerateLocalId()
    {
    }
    void SteamIdentity::GenerateLocalUsername()
    {
    }
    std::vector<uint8_t> SteamIdentity::GetAuthToken(uint64_t /*remoteId*/)
    {
        return {};
    }
    bool SteamIdentity::ValidateToken(uint64_t /*id*/, const std::vector<uint8_t>& /*token*/)
    {
        return false;
    }
    void SteamIdentity::OnDisconnect(uint64_t /*id*/)
    {
    }
    void SteamIdentity::Clean()
    {
    }

#endif

}