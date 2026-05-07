#pragma once

#include <string>
#include <vector>

#include "igneous/networking/IdentityProvider.hpp"

#ifdef IGNEOUS_STEAM_ENABLED
#include <steam/steam_api.h>
#endif

namespace Engine
{
    /**
     * @class SteamIdentity
     * @brief Steamworks-backed identity and authentication provider.
     *
     * SteamIdentity implements IdentityProvider using the Steamworks API.
     * It retrieves the local user's SteamID and persona name, generates
     * auth session tickets for connecting to servers, and validates incoming
     * tickets on the server side via ISteamGameServer::BeginAuthSession.
     *
     * The OnAuthResult callback is fired asynchronously when Steam finishes
     * validating a ticket via the ValidateAuthTicketResponse_t callback.
     *
     * @note This class requires IGNEOUS_STEAM_ENABLED to be defined at compile
     * time (set automatically when IGNEOUS_STEAM is ON in CMake). Without it,
     * all methods are stubs that do nothing or return empty/zero values.
     */
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

        /**
         * @brief Returns the local user's SteamID as a uint64.
         *
         * Returns 0 if Steamworks is not enabled or GenerateLocalId() has
         * not been called yet.
         */
        uint64_t GetLocalId() override;

        /**
         * @brief Returns the local user's Steam persona name.
         *
         * Returns an empty string if Steamworks is not enabled or
         * GenerateLocalUsername() has not been called yet.
         */
        std::string GetLocalUsername() override;

        /**
         * @brief Fetches and caches the local SteamID from the Steamworks API.
         */
        void GenerateLocalId() override;

        /**
         * @brief Fetches and caches the local persona name from the Steamworks API.
         */
        void GenerateLocalUsername() override;

        /**
         * @brief Generates a Steam auth session ticket for the given remote server.
         *
         * The ticket is bound to the server's SteamID via SteamNetworkingIdentity,
         * preventing replay attacks against other servers.
         *
         * @param remoteId SteamID of the server being connected to.
         * @return Raw ticket bytes to send to the server, or empty on failure.
         */
        std::vector<uint8_t> GetAuthToken(uint64_t remoteId) override;

        /**
         * @brief Begins a Steam auth session to validate a client's ticket.
         *
         * This is a server-side call. The result is delivered asynchronously
         * via the OnAuthResult callback once Steam responds.
         *
         * @param claimedId The SteamID the client claims to own.
         * @param token     The raw ticket bytes received from the client.
         * @return true if the session was started successfully (not yet validated),
         *         false if the ticket was immediately rejected.
         */
        bool ValidateToken(uint64_t claimedId, const std::vector<uint8_t>& token) override;

        /**
         * @brief Ends the auth session for a disconnected client.
         *
         * Must be called when a client disconnects to free Steamworks resources.
         *
         * @param id SteamID of the disconnected client.
         */
        void OnDisconnect(uint64_t id) override;

        /**
         * @brief Cancels the outstanding auth ticket, if any.
         */
        void Clean() override;
    };
}