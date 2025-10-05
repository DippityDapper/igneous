#include "dapper2d/CFGParser.hpp"

#include <sstream>
#include <fstream>

#include "SDL3/SDL.h"

namespace Engine
{
    std::map<std::string, std::map<std::string, std::string>> CFGParser::configs;

    void CFGParser::LoadConfig(std::string& fileName)
    {
        std::string fullPath = "configs/" + fileName;
        std::ifstream serverPropertiesFile(fullPath);

        if (!serverPropertiesFile.is_open())
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load server config file");
            return;
        }

        configs[fileName] = {};
        std::string line;

        while (std::getline(serverPropertiesFile, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            auto delimiterPos = line.find('=');
            if (delimiterPos == std::string::npos)
                continue;

            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);

            configs[fileName][key] = value;
        }
    }

    std::string CFGParser::GetString(std::string& configName, std::string& key)
    {
        if (!configs.contains(configName))
            return "";

        if (!configs[configName].contains(key))
            return "";

        return configs[configName][key];
    }

    int CFGParser::GetInt(std::string& configName, std::string& key)
    {
        if (!configs.contains(configName))
            return 0;

        if (!configs[configName].contains(key))
            return 0;

        std::string val = configs[configName][key];

        try
        {
            int num = std::stoi(val);
            return num;
        }
        catch (const std::invalid_argument&)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Config Parser could not parse int for key %s in %s", val.c_str(), configName.c_str());
            return 0;
        }
    }

    int CFGParser::GetBool(std::string& configName, std::string& key)
    {
        if (!configs.contains(configName))
            return 0;

        if (!configs[configName].contains(key))
            return 0;

        std::string val = configs[configName][key];

        std::istringstream ss(val);

        bool value = false;
        if (!(ss >> std::boolalpha >> value))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Config Parser could not parse bool for key %s in %s", val.c_str(), configName.c_str());
            return false;
        }

        return value;
    }
}
