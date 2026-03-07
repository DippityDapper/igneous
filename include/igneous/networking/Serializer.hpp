#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>
#include <boost/pfr.hpp>

namespace Engine
{
    /**
     * @brief Concept that identifies aggregate structs eligible for automatic
     * field-by-field serialization via Boost.PFR.
     *
     * A type satisfies SerializableStruct if it meets ALL of the following:
     *
     * 1. Aggregate:
     *    - No user-declared constructors
     *    - No private or protected non-static data members
     *    - No base classes
     *    - No virtual functions
     *
     * 2. Not arithmetic:
     *    - Excludes int, float, double, bool, char, etc.
     *
     * 3. Not std::string (`!std::is_same_v<T, std::string>`):
     *
     * All fields within the struct must themselves be serializable types:
     *    - Arithmetic types      (uint16_t, float, int64_t, etc.)
     *    - std::string
     *    - Nested SerializableStructs
     *    - std::vector<T> where T is arithmetic or a SerializableStruct
     *
     * @note Field declaration order determines wire format order. Never reorder fields without updating all senders and receivers.
     */
    template<typename T>
    concept SerializableStruct =
            std::is_aggregate_v<T> &&
            !std::is_arithmetic_v<T> &&
            !std::is_same_v<T, std::string>;

    /**
     * @class Serializer
     * @brief Serializes data into a byte buffer.
     *
     * Serializer allows writing arithmetic types, strings, and vectors into
     * a contiguous byte buffer. This is typically used for network transmission
     * or file storage.
     *
     * Example usage:
     * @code
     * std::vector<uint8_t> buffer;
     * Serializer ser(buffer);
     * int x = 42;
     * std::string name = "Player";
     * ser << x << name;
     * @endcode
     */
    class Serializer
    {
      private:
        /**
         * @brief Buffer to write serialized data into
         */
        std::vector<uint8_t>& buffer;

      public:
        /**
         * @brief Constructs a Serializer targeting the given buffer.
         * @param buf Buffer to append serialized bytes to.
         */
        explicit Serializer(std::vector<uint8_t>& buf) : buffer(buf)
        {
        }

        /**
         * @brief Serializes an arithmetic value (int, float, etc.).
         * @tparam T Numeric type.
         * @param value Value to serialize.
         * @return Reference to self for chaining.
         */
        template<typename T>
        requires std::is_arithmetic_v<T>
        Serializer& operator<<(const T& value)
        {
            const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
            buffer.insert(buffer.end(), bytes, bytes + sizeof(T));
            return *this;
        }

        /**
         * @brief Serializes a string.
         *
         * The string is stored as a 32-bit length prefix followed by the characters.
         *
         * @param str String to serialize.
         * @return Reference to self for chaining.
         */
        Serializer& operator<<(const std::string& str)
        {
            uint32_t len = str.size();
            *this << len;
            buffer.insert(buffer.end(), str.begin(), str.end());
            return *this;
        }

        /**
         * @brief Serializes a vector of arithmetic values.
         *
         * The vector is stored as a 32-bit length prefix followed by each element.
         *
         * @tparam T Numeric type.
         * @param vec Vector to serialize.
         * @return Reference to self for chaining.
         */
        template<typename T>
        requires std::is_arithmetic_v<T>
        Serializer& operator<<(const std::vector<T>& vec)
        {
            uint32_t count = vec.size();
            *this << count;
            for (const auto& item: vec)
                *this << item;
            return *this;
        }

        /**
         * @brief Serializes a vector of serializable structs.
         *
         * The vector is stored as a 32-bit length prefix followed by each element.
         *
         * @tparam T Serializable structure type.
         * @param vec Vector to serialize.
         * @return Reference to self for chaining.
         */
        template<SerializableStruct T>
        Serializer& operator<<(const std::vector<T>& vec)
        {
            uint32_t count = static_cast<uint32_t>(vec.size());
            *this << count;
            for (const auto& item: vec)
                *this << item;
            return *this;
        }

        /**
         * @brief Serializes a serializable struct.
         * @tparam T Serializable structure type.
         * @param obj Serializable structure to serialize.
         * @return Reference to self for chaining.
         */
        template<SerializableStruct T>
        Serializer& operator<<(const T& obj)
        {
            boost::pfr::for_each_field(obj, [this](const auto& field)
                                       { *this << field; });
            return *this;
        }
    };

    /**
     * @class Deserializer
     * @brief Deserializes data from a byte buffer.
     *
     * Deserializer allows reading arithmetic types, strings, and vectors from
     * a previously serialized buffer. It maintains an internal offset
     * to track the current read position.
     *
     * Example usage:
     * @code
     * Deserializer des(buffer);
     * int x;
     * std::string name;
     * des >> x >> name;
     * @endcode
     */
    class Deserializer
    {
      private:
        /**
         * @brief Buffer to read serialized data from
         */
        const std::vector<uint8_t>& buffer;

        /**
         * @brief Current read position
         */
        size_t offset;

      public:
        /**
         * @brief Constructs a Deserializer for a given buffer.
         * @param buf Buffer to read from.
         * @param startOffset Initial offset within the buffer (default: 1).
         */
        explicit Deserializer(const std::vector<uint8_t>& buf, size_t startOffset = 1)
            : buffer(buf), offset(startOffset)
        {
        }

        /**
         * @brief Deserializes an arithmetic value (int, float, etc.).
         * @tparam T Numeric type.
         * @param value Reference to store the deserialized value.
         * @return Reference to self for chaining.
         */
        template<typename T>
        requires std::is_arithmetic_v<T>
        Deserializer& operator>>(T& value)
        {
            std::memcpy(&value, &buffer[offset], sizeof(T));
            offset += sizeof(T);
            return *this;
        }

        /**
         * @brief Deserializes a string.
         *
         * Reads a 32-bit length prefix followed by the characters.
         *
         * @param str Reference to store the deserialized string.
         * @return Reference to self for chaining.
         */
        Deserializer& operator>>(std::string& str)
        {
            uint32_t len;
            *this >> len;
            str.assign(reinterpret_cast<const char*>(&buffer[offset]), len);
            offset += len;
            return *this;
        }

        /**
         * @brief Deserializes a vector of arithmetic values.
         *
         * Reads a 32-bit length prefix followed by each element.
         *
         * @tparam T Numeric type.
         * @param vec Reference to store the deserialized vector.
         * @return Reference to self for chaining.
         */
        template<typename T>
        requires std::is_arithmetic_v<T>
        Deserializer& operator>>(std::vector<T>& vec)
        {
            uint32_t count;
            *this >> count;
            vec.resize(count);
            for (auto& item: vec)
                *this >> item;
            return *this;
        }

        /**
         * @brief Deserializes a vector of serializable structs.
         *
         * Reads a 32-bit length prefix followed by each element.
         *
         * @tparam T Serializable structure type.
         * @param vec Reference to store the deserialized vector.
         * @return Reference to self for chaining.
         */
        template<SerializableStruct T>
        Deserializer& operator>>(std::vector<T>& vec)
        {
            uint32_t count;
            *this >> count;
            vec.resize(count);
            for (auto& item: vec)
                *this >> item;
            return *this;
        }

        /**
         * @brief Deserializes a serializable struct.
         * @tparam T Serializable structure type.
         * @param obj Reference to store the deserialized value.
         * @return Reference to self for chaining.
         */
        template<SerializableStruct T>
        Deserializer& operator>>(T& obj)
        {
            boost::pfr::for_each_field(obj, [this](auto& field)
                                       { *this >> field; });
            return *this;
        }

        /**
         * @brief Returns the current read offset in the buffer.
         * @return Current offset as a size_t.
         */
        size_t GetOffset() const
        {
            return offset;
        }
    };
}
