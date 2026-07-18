// Doc: docs/classes/CFGParser.md
#pragma once

#include <map>
#include <string>
#include <cstdint>

namespace Engine
{

    class CFGParser
    {
      private:

        static inline std::map<std::string, std::map<std::string, std::string>> configs;

      public:

        CFGParser() = delete;

        CFGParser(const CFGParser& other) = delete;

        /// Clears loaded configs between unit tests.
        static void ResetForTests();

        static void LoadConfig(const std::string& filePath, const std::string& configName);

        static void SaveConfig(const std::string& filePath, const std::string& configName);

        static std::string GetString(const std::string& configName, const std::string& key);

        static int GetInt(const std::string& configName, const std::string& key);

        static uint32_t GetUInt32(const std::string& configName, const std::string& key);

        static uint16_t GetUInt16(const std::string& configName, const std::string& key);

        static bool GetBool(const std::string& configName, const std::string& key);

        static void WriteString(const std::string& configName, const std::string& key, const std::string& value);

        static void WriteInt(const std::string& configName, const std::string& key, int value);

        static void WriteUInt32(const std::string& configName, const std::string& key, uint32_t value);

        static void WriteUInt16(const std::string& configName, const std::string& key, uint16_t value);

        static void WriteBool(const std::string& configName, const std::string& key, bool value);
    };
}
