#pragma once
#include <cstdint>
#include <functional>
#include <string>

namespace Engine
{
    class IdentityProvider
    {
      public:
        virtual ~IdentityProvider() = default;

        // Event: OnAuthResult(userId, success)
        std::function<void(uint64_t, bool)> OnAuthResult;

        virtual uint64_t GetLocalId() = 0;
        virtual std::string GetLocalUsername() = 0;
        virtual void GenerateLocalId() = 0;
        virtual void GenerateLocalUsername() = 0;
        virtual std::vector<uint8_t> GetAuthToken(uint64_t remoteId) = 0;
        virtual bool ValidateToken(uint64_t claimedId, const std::vector<uint8_t>& token) = 0;
        virtual void OnDisconnect(uint64_t id) = 0;
        virtual void Clean() = 0;
    };
}