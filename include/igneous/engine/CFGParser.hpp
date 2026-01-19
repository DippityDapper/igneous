#pragma once

#include <map>
#include <string>
#include <cstdint>

namespace Engine
{
    /**
     * @class CFGParser
     * @brief A utility class for loading and managing configuration files.
     *
     * CFGParser provides static methods for reading and writing key-value pair
     * configuration files. It supports loading multiple named configurations
     * simultaneously, parsing values as various types (string, int, uint32_t,
     * uint16_t, bool), and saving modified configurations back to disk.
     *
     * Configuration files use a simple key=value format with support for comments.
     * All whitespace around keys and values is automatically stripped during parsing.
     *
     * Example configuration file format:
     * @code
     * # This is a comment
     * world_size=small
     * chunk_size = 8
     * debug_mode = true
     * @endcode
     *
     * @note This class is non-instantiable. All methods are static.
     * @note Configurations are stored in memory and persist until the application exits.
     */
    class CFGParser
    {
      private:
        /**
         * @brief Internal storage for all loaded configurations.
         *
         * Maps configuration names to their key-value pairs. Each configuration
         * is identified by a unique string name and contains a map of string
         * key-value pairs.
         */
        static inline std::map<std::string, std::map<std::string, std::string>> configs;

      public:
        /**
         * @brief Deleted default constructor.
         */
        CFGParser() = delete;

        /**
         * @brief Deleted copy constructor.
         */
        CFGParser(const CFGParser& other) = delete;

        /**
         * @brief Loads a configuration file and parses its key-value pairs.
         *
         * Reads a file containing key-value pairs in the format key=value.
         * Comments starting with '#' are ignored. Whitespace around keys and
         * values is automatically removed. If the file doesn't exist, it will
         * be created as an empty file.
         *
         * File format example:
         * @code
         * # This is a comment
         * world_size=small
         * chunk_size = 8       # Inline comments are supported
         * debug _ mode=true    # Whitespace in keys is removed
         * @endcode
         *
         * @param filePath Path to the configuration file.
         * @param configName Unique identifier used to reference this configuration.
         */
        static void LoadConfig(const std::string& filePath, const std::string& configName);

        /**
         * @brief Saves the in-memory configuration to a file.
         *
         * Writes all key-value pairs from the specified configuration to disk.
         * The configuration must have been previously loaded or created via
         * Write methods.
         *
         * @param filePath Path where the configuration file will be saved.
         * @param configName Identifier of the configuration to save.
         */
        static void SaveConfig(const std::string& filePath, const std::string& configName);

        /**
         * @brief Retrieves a configuration value as a string.
         *
         * @param configName Identifier of the configuration.
         * @param key The key to retrieve.
         * @return The value as a string.
         *
         * @throw std::runtime_error if the configuration or key doesn't exist.
         */
        static std::string GetString(const std::string& configName, const std::string& key);

        /**
         * @brief Retrieves a configuration value as an integer.
         *
         * @param configName Identifier of the configuration.
         * @param key The key to retrieve.
         * @return The value parsed as an integer.
         *
         * @throw std::runtime_error if the configuration or key doesn't exist,
         *        or if the value cannot be parsed as an integer.
         */
        static int GetInt(const std::string& configName, const std::string& key);

        /**
         * @brief Retrieves a configuration value as a uint32_t.
         *
         * @param configName Identifier of the configuration.
         * @param key The key to retrieve.
         * @return The value parsed as a uint32_t, or 0 if parsing fails.
         *
         * @throw std::runtime_error if the configuration or key doesn't exist.
         */
        static uint32_t GetUInt32(const std::string& configName, const std::string& key);

        /**
         * @brief Retrieves a configuration value as a uint16_t.
         *
         * @param configName Identifier of the configuration.
         * @param key The key to retrieve.
         * @return The value parsed as a uint16_t, or 0 if parsing fails.
         *
         * @throw std::runtime_error if the configuration or key doesn't exist.
         */
        static uint32_t GetUInt16(const std::string& configName, const std::string& key);

        /**
         * @brief Retrieves a configuration value as a boolean.
         *
         * Parses the value using boolalpha format (expects "true" or "false").
         *
         * @param configName Identifier of the configuration.
         * @param key The key to retrieve.
         * @return The value parsed as a boolean, or false if parsing fails.
         *
         * @throw std::runtime_error if the configuration or key doesn't exist.
         */
        static bool GetBool(const std::string& configName, const std::string& key);

        /**
         * @brief Writes a string value to the in-memory configuration.
         *
         * If the configuration doesn't exist, it will be created. Changes are
         * not persisted to disk until SaveConfig() is called.
         *
         * @param configName Identifier of the configuration.
         * @param key The key to write.
         * @param value The string value to store.
         */
        static void WriteString(const std::string& configName, const std::string& key, const std::string& value);

        /**
         * @brief Writes an integer value to the in-memory configuration.
         *
         * The value is converted to a string for storage. If the configuration
         * doesn't exist, it will be created.
         *
         * @param configName Identifier of the configuration.
         * @param key The key to write.
         * @param value The integer value to store.
         */
        static void WriteInt(const std::string& configName, const std::string& key, int value);

        /**
         * @brief Writes a uint32_t value to the in-memory configuration.
         *
         * The value is converted to a string for storage. If the configuration
         * doesn't exist, it will be created.
         *
         * @param configName Identifier of the configuration.
         * @param key The key to write.
         * @param value The uint32_t value to store.
         */
        static void WriteUInt32(const std::string& configName, const std::string& key, uint32_t value);

        /**
         * @brief Writes a uint16_t value to the in-memory configuration.
         *
         * The value is converted to a string for storage. If the configuration
         * doesn't exist, it will be created.
         *
         * @param configName Identifier of the configuration.
         * @param key The key to write.
         * @param value The uint16_t value to store.
         */
        static void WriteUInt16(const std::string& configName, const std::string& key, uint16_t value);

        /**
         * @brief Writes a boolean value to the in-memory configuration.
         *
         * The value is stored in boolalpha format ("true" or "false"). If the
         * configuration doesn't exist, it will be created.
         *
         * @param configName Identifier of the configuration.
         * @param key The key to write.
         * @param value The boolean value to store.
         */
        static void WriteBool(const std::string& configName, const std::string& key, bool value);
    };
}