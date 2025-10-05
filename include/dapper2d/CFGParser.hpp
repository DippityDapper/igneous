#pragma once

#include <map>
#include <string>

namespace Engine
{
    class CFGParser
    {
    private:
        static std::map<std::string, std::map<std::string, std::string>> configs;

    public:
        static void LoadConfig(std::string& fileName);

        static std::string GetString(std::string& configName, std::string& key);
        static int GetInt(std::string& configName, std::string& key);
        static int GetBool(std::string& configName, std::string& key);
    };
}
