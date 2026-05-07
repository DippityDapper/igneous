#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>
#include <functional>
#include <stdexcept>

namespace Engine
{
    /**
     * @class Serializer
     * @brief Serializes data into an owned byte buffer.
     *
     * Mirrors the C# Serializer API: explicit Write() overloads per type,
     * and a callback-based Write() for collections.
     *
     * Example usage:
     * @code
     * Serializer ser;
     * ser.Write((int32_t)42).Write(std::string("Player"));
     * std::vector<uint8_t> bytes = ser.GetBytes();
     * @endcode
     */
    class Serializer
    {
      private:
        std::vector<uint8_t> _buffer;

        template<typename T>
        requires std::is_arithmetic_v<T>
        Serializer& WriteRaw(T value)
        {
            const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
            _buffer.insert(_buffer.end(), bytes, bytes + sizeof(T));
            return *this;
        }

      public:
        Serializer& Write(bool value)
        {
            return WriteRaw(value);
        }
        Serializer& Write(uint8_t value)
        {
            return WriteRaw(value);
        }
        Serializer& Write(int16_t value)
        {
            return WriteRaw(value);
        }
        Serializer& Write(uint16_t value)
        {
            return WriteRaw(value);
        }
        Serializer& Write(int32_t value)
        {
            return WriteRaw(value);
        }
        Serializer& Write(uint32_t value)
        {
            return WriteRaw(value);
        }
        Serializer& Write(int64_t value)
        {
            return WriteRaw(value);
        }
        Serializer& Write(uint64_t value)
        {
            return WriteRaw(value);
        }
        Serializer& Write(float value)
        {
            return WriteRaw(value);
        }
        Serializer& Write(double value)
        {
            return WriteRaw(value);
        }

        /**
         * @brief Serializes a string as a 32-bit length prefix followed by UTF-8 bytes.
         *
         * @note Matches C#'s BinaryWriter.Write(string) 7-bit encoded length prefix for
         * ASCII/short strings, but here we use a flat uint32 for simplicity and
         * cross-language compatibility. Keep senders and receivers in sync.
         */
        Serializer& Write(const std::string& value)
        {
            Write(static_cast<uint32_t>(value.size()));
            _buffer.insert(_buffer.end(), value.begin(), value.end());
            return *this;
        }

        /**
         * @brief Serializes a byte array as a 32-bit length prefix followed by raw bytes.
         */
        Serializer& Write(const std::vector<uint8_t>& value)
        {
            Write(static_cast<int32_t>(value.size()));
            _buffer.insert(_buffer.end(), value.begin(), value.end());
            return *this;
        }

        /**
         * @brief Serializes a list of T using a caller-supplied write callback.
         *
         * The list is stored as a 32-bit element count followed by each element
         * as written by writeItem.
         *
         * Example:
         * @code
         * std::vector<Player> players = ...;
         * ser.Write<Player>(players, [&](const Player& p) {
         *     ser.Write(p.name).Write(p.score);
         * });
         * @endcode
         *
         * @tparam T Element type.
         * @param list  Collection to serialize.
         * @param writeItem Callback invoked once per element.
         */
        template<typename T>
        Serializer& Write(const std::vector<T>& list, std::function<void(const T&)> writeItem)
        {
            Write(static_cast<int32_t>(list.size()));
            for (const T& item: list)
                writeItem(item);
            return *this;
        }

        /**
         * @brief Returns a copy of the serialized bytes.
         */
        std::vector<uint8_t> GetBytes() const
        {
            return _buffer;
        }
    };

    /**
     * @class Deserializer
     * @brief Deserializes data from a byte buffer.
     *
     * Mirrors the C# Deserializer API: explicit ReadXxx() methods per type,
     * and a callback-based ReadList() for collections.
     *
     * Example usage:
     * @code
     * Deserializer des(buffer);
     * int32_t x  = des.ReadInt();
     * std::string name = des.ReadString();
     * @endcode
     */
    class Deserializer
    {
      private:
        const std::vector<uint8_t>& _buffer;
        size_t _offset;

        template<typename T>
        requires std::is_arithmetic_v<T>
        T ReadRaw()
        {
            if (_offset + sizeof(T) > _buffer.size())
                throw std::out_of_range("Deserializer: read past end of buffer");
            T value;
            std::memcpy(&value, &_buffer[_offset], sizeof(T));
            _offset += sizeof(T);
            return value;
        }

      public:
        explicit Deserializer(const std::vector<uint8_t>& buf, size_t startOffset = 2)
            : _buffer(buf), _offset(startOffset)
        {
        }

        bool ReadBool()
        {
            return ReadRaw<bool>();
        }
        uint8_t ReadByte()
        {
            return ReadRaw<uint8_t>();
        }
        int16_t ReadShort()
        {
            return ReadRaw<int16_t>();
        }
        uint16_t ReadUShort()
        {
            return ReadRaw<uint16_t>();
        }
        int32_t ReadInt()
        {
            return ReadRaw<int32_t>();
        }
        uint32_t ReadUInt()
        {
            return ReadRaw<uint32_t>();
        }
        int64_t ReadLong()
        {
            return ReadRaw<int64_t>();
        }
        uint64_t ReadULong()
        {
            return ReadRaw<uint64_t>();
        }
        float ReadFloat()
        {
            return ReadRaw<float>();
        }
        double ReadDouble()
        {
            return ReadRaw<double>();
        }

        /**
         * @brief Deserializes a string from a 32-bit length prefix followed by UTF-8 bytes.
         */
        std::string ReadString()
        {
            uint32_t len = ReadUInt();
            if (_offset + len > _buffer.size())
                throw std::out_of_range("Deserializer: string read past end of buffer");
            std::string str(reinterpret_cast<const char*>(&_buffer[_offset]), len);
            _offset += len;
            return str;
        }

        /**
         * @brief Deserializes a byte array from a 32-bit length prefix followed by raw bytes.
         */
        std::vector<uint8_t> ReadBytes()
        {
            int32_t count = ReadInt();
            if (count < 0 || _offset + count > _buffer.size())
                throw std::out_of_range("Deserializer: byte array read past end of buffer");
            std::vector<uint8_t> bytes(&_buffer[_offset], &_buffer[_offset] + count);
            _offset += count;
            return bytes;
        }

        /**
         * @brief Deserializes a list of T using a caller-supplied read callback.
         *
         * Reads a 32-bit element count, then invokes readItem() that many times
         * to populate the returned vector.
         *
         * Example:
         * @code
         * auto players = des.ReadList<Player>([&]() {
         *     Player p;
         *     p.name  = des.ReadString();
         *     p.score = des.ReadInt();
         *     return p;
         * });
         * @endcode
         *
         * @tparam T Element type.
         * @param readItem Callback invoked once per element, returning T.
         * @return Populated vector of T.
         */
        template<typename T>
        std::vector<T> ReadList(std::function<T()> readItem)
        {
            int32_t count = ReadInt();
            std::vector<T> list;
            list.reserve(count);
            for (int32_t i = 0; i < count; i++)
                list.push_back(readItem());
            return list;
        }

        size_t GetOffset() const
        {
            return _offset;
        }
    };
}