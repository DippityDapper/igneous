#include "igneous/CFGParser.hpp"

#include <sstream>
#include <fstream>
#include <algorithm>

#include "SDL3/SDL.h"

namespace Engine
{

    std::map<std::string, std::map<std::string, std::string>> CFGParser::configs;

    void CFGParser::LoadConfig(const std::string& filePath, const std::string& configName)
    {
        std::ifstream configFile(filePath);

        if (!configFile.is_open())
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open config file : %s", filePath.c_str());
            return;
        }

        configs[configName] = {};
        std::string line;

        while (std::getline(configFile, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos)
                line.erase(line.begin() + commentPos, line.end());

            size_t delimiterPos = line.find('=');
            if (delimiterPos == std::string::npos)
                continue;

            std::string key = line.substr(0, delimiterPos);
            key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());

            std::string value = line.substr(delimiterPos + 1);
            value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());

            configs[configName][key] = value;
        }
    }

    std::string CFGParser::GetString(const std::string& configName, const std::string& key)
    {
        if (!configs.contains(configName))
            throw std::runtime_error("Configs does not contain the config " + configName);
        if (!configs[configName].contains(key))
            throw std::runtime_error("Config " + configName + " does not contain the key " + key);

        return configs[configName][key];
    }

    int CFGParser::GetInt(const std::string& configName, const std::string& key)
    {
        if (!configs.contains(configName))
            throw std::runtime_error("Configs does not contain the config " + configName);
        if (!configs[configName].contains(key))
            throw std::runtime_error("Config " + configName + " does not contain the key " + key);

        std::string val = configs[configName][key];

        try
        {
            int num = std::stoi(val);
            return num;
        }
        catch (const std::invalid_argument&)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Config Parser could not parse int for key %s in %s", val.c_str(), configName.c_str());
            throw std::runtime_error("Could not parse key " + key + " as int for config " + configName);
        }
    }

    uint32_t CFGParser::GetUInt32(const std::string& configName, const std::string& key)
    {
        if (!configs.contains(configName))
            throw std::runtime_error("Configs does not contain the config " + configName);
        if (!configs[configName].contains(key))
            throw std::runtime_error("Config " + configName + " does not contain the key " + key);

        std::string val = configs[configName][key];

        try
        {
            uint32_t num = std::stoul(val);
            return num;
        }
        catch (const std::invalid_argument&)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Config Parser could not parse int for key %s in %s", val.c_str(), configName.c_str());
            return 0;
        }
    }

    uint32_t CFGParser::GetUInt16(const std::string& configName, const std::string& key)
    {
        if (!configs.contains(configName))
            throw std::runtime_error("Configs does not contain the config " + configName);
        if (!configs[configName].contains(key))
            throw std::runtime_error("Config " + configName + " does not contain the key " + key);

        std::string val = configs[configName][key];

        try
        {
            uint32_t num = std::stoul(val);
            return static_cast<uint16_t>(num);
        }
        catch (const std::invalid_argument&)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Config Parser could not parse int for key %s in %s", val.c_str(), configName.c_str());
            return 0;
        }
    }

    bool CFGParser::GetBool(const std::string& configName, const std::string& key)
    {
        if (!configs.contains(configName))
            throw std::runtime_error("Configs does not contain the config " + configName);
        if (!configs[configName].contains(key))
            throw std::runtime_error("Config " + configName + " does not contain the key " + key);

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
