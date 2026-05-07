#include "igneous/networking/LocalIdentity.hpp"
#include <fstream>
#include <cstring>

namespace Engine
{
    LocalIdentity::LocalIdentity(const std::string& userFilePath)
        : _userFilePath(userFilePath)
    {
    }

    bool LocalIdentity::ParseUserFile(const std::string& key, std::string& outValue) const
    {
        std::ifstream file(_userFilePath);
        if (!file.is_open())
            return false;

        std::string line;
        while (std::getline(file, line))
        {
            auto sep = line.find('=');
            if (sep == std::string::npos)
                continue;

            std::string k = line.substr(0, sep);
            std::string v = line.substr(sep + 1);
            if (k == key)
            {
                outValue = v;
                return true;
            }
        }
        return false;
    }

    uint64_t LocalIdentity::GetLocalId()
    {
        return _localId;
    }

    std::string LocalIdentity::GetLocalUsername()
    {
        return _localUsername;
    }

    void LocalIdentity::GenerateLocalId()
    {
        std::string value;
        if (ParseUserFile("id", value))
        {
            try
            {
                _localId = std::stoull(value);
            }
            catch (...)
            {
                _localId = 0;
            }
        }
    }

    void LocalIdentity::GenerateLocalUsername()
    {
        std::string value;
        if (ParseUserFile("username", value))
            _localUsername = value;
    }

    std::vector<uint8_t> LocalIdentity::GetAuthToken(uint64_t remoteId)
    {
        uint64_t id = GetLocalId();
        std::vector<uint8_t> token(sizeof(uint64_t));
        std::memcpy(token.data(), &id, sizeof(uint64_t));
        return token;
    }

    bool LocalIdentity::ValidateToken(uint64_t claimedId, const std::vector<uint8_t>& token)
    {
        if (OnAuthResult)
            OnAuthResult(claimedId, true);
        return true;
    }

    void LocalIdentity::OnDisconnect(uint64_t id)
    {
    }

    void LocalIdentity::Clean()
    {
    }
}