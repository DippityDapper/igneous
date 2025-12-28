#pragma once

#include <map>
#include <string>
#include <cstdint>

namespace Engine
{
    /// A utility class that provides static methods for loading and accessing configuration files.
    /// The parser reads key-value pairs and stores them by configuration name.
    class CFGParser
    {
    private:
        /// A map of the parsed configs.
        static std::map<std::string, std::map<std::string, std::string>> configs;

    public:
        CFGParser() = delete;
        CFGParser(const CFGParser& other) = delete;

        /// Loads a file with key value pairs to be parsed. The structure of the file should follow the example below.
        /// @code
        /// # File structure. This line is also a valid comment within the file.
        /// world_size=small
        /// chunk_size = 8 # White space is removed.
        /// debug _ mode=true # The white space around _ is removed.
        /// @endcode
        /// @param filePath The path of the config file.
        /// @param configName The string used to reference the loaded config.
        static void LoadConfig(const std::string& filePath, const std::string& configName);

        /// Gets the value of a config as a string.
        /// @param configName The reference string of the config file.
        /// @param key The config to obtain the value of.
        /// @returns The config value as a string.
        static std::string GetString(const std::string& configName, const std::string& key);

        /// Gets the value of a config as an integer.
        /// @param configName The reference string of the config file.
        /// @param key The config to obtain the value of.
        /// @returns The config value as an integer.
        static int GetInt(const std::string& configName, const std::string& key);

        /// Gets the value of a config as a uint32.
        /// @param configName The reference string of the config file.
        /// @param key The config to obtain the value of.
        /// @returns The config value as a uint32.
        static uint32_t GetUInt32(const std::string& configName, const std::string& key);

        /// Gets the value of a config as a uint16.
        /// @param configName The reference string of the config file.
        /// @param key The config to obtain the value of.
        /// @returns The config value as a uint16.
        static uint32_t GetUInt16(const std::string& configName, const std::string& key);

        /// Gets the value of a config as a bool.
        /// @param configName The reference string of the config file.
        /// @param key The config to obtain the value of.
        /// @returns The config value as a bool.
        static bool GetBool(const std::string& configName, const std::string& key);
    };
}
