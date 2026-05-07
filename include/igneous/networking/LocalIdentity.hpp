#pragma once
#include "igneous/networking/IdentityProvider.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace Engine
{
    class LocalIdentity : public IdentityProvider
    {
      public:
        explicit LocalIdentity(const std::string& userFilePath);

        uint64_t GetLocalId() override;
        std::string GetLocalUsername() override;
        void GenerateLocalId() override;
        void GenerateLocalUsername() override;
        std::vector<uint8_t> GetAuthToken(uint64_t remoteId) override;
        bool ValidateToken(uint64_t claimedId, const std::vector<uint8_t>& token) override;
        void OnDisconnect(uint64_t id) override;
        void Clean() override;

      private:
        uint64_t _localId = 0;
        std::string _localUsername;
        std::string _userFilePath;

        bool ParseUserFile(const std::string& key, std::string& outValue) const;
    };
}