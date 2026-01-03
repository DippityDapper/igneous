#include "igneous/CFGParser.hpp"

#include <sstream>
#include <fstream>
#include <algorithm>

#include "SDL3/SDL.h"

namespace Engine
{
    void CFGParser::LoadConfig(const std::string& filePath, const std::string& configName)
    {
        std::ifstream configFile(filePath);

        if (!configFile.is_open())
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Config file not found, creating: %s", filePath.c_str());

            std::ofstream newFile(filePath);
            if (!newFile.is_open())
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create config file: %s", filePath.c_str());
                return;
            }

            newFile.close();

            configFile.open(filePath);
            if (!configFile.is_open())
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open newly created config file: %s", filePath.c_str());
                return;
            }
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
            std::erase_if(key, isspace);

            std::string value = line.substr(delimiterPos + 1);
            std::erase_if(value, isspace);

            configs[configName][key] = value;
        }

        configFile.close();
    }

    void CFGParser::SaveConfig(const std::string& filePath, const std::string& configName)
    {
        if (!configs.contains(configName))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Cannot save config %s: config not loaded", configName.c_str());
            return;
        }

        std::ofstream configFile(filePath);
        if (!configFile.is_open())
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open config file for writing: %s", filePath.c_str());
            return;
        }

        for (const auto& [key, value] : configs[configName])
        {
            configFile << key << "=" << value << "\n";
        }

        configFile.close();
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

    void CFGParser::WriteString(const std::string& configName, const std::string& key, const std::string& value)
    {
        if (!configs.contains(configName))
            configs[configName] = {};

        configs[configName][key] = value;
    }

    void CFGParser::WriteInt(const std::string& configName, const std::string& key, int value)
    {
        if (!configs.contains(configName))
            configs[configName] = {};

        configs[configName][key] = std::to_string(value);
    }

    void CFGParser::WriteUInt32(const std::string& configName, const std::string& key, uint32_t value)
    {
        if (!configs.contains(configName))
            configs[configName] = {};

        configs[configName][key] = std::to_string(value);
    }

    void CFGParser::WriteUInt16(const std::string& configName, const std::string& key, uint16_t value)
    {
        if (!configs.contains(configName))
            configs[configName] = {};

        configs[configName][key] = std::to_string(value);
    }

    void CFGParser::WriteBool(const std::string& configName, const std::string& key, bool value)
    {
        if (!configs.contains(configName))
            configs[configName] = {};

        std::ostringstream ss;
        ss << std::boolalpha << value;
        configs[configName][key] = ss.str();
    }
}