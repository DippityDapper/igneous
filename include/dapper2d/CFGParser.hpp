#pragma once

#include <map>
#include <string>
#include <cstdint>

namespace Engine
{
    class CFGParser
    {
    private:
        static std::map<std::string, std::map<std::string, std::string>> configs;

    public:
        static void LoadConfig(const std::string& fileName);

        static std::string GetString(const std::string& configName, const std::string& key);
        static int GetInt(const std::string& configName, const std::string& key);
        static uint32_t GetUInt32(const std::string& configName, const std::string& key);
        static int GetBool(const std::string& configName, const std::string& key);
    };
}
