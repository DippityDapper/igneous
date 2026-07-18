// Doc: docs/classes/Serializer.md
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>
#include <functional>
#include <stdexcept>

#include "igneous/networking/NetworkProtocol.hpp"

namespace Engine
{

    /// Binary serializer for network payloads.
    ///
    /// Arithmetic types are written with native endianness via `reinterpret_cast`.
    /// Messages are **not** portable across endianness without an explicit conversion layer.
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

        Serializer& Write(const std::string& value)
        {
            Write(static_cast<uint32_t>(value.size()));
            _buffer.insert(_buffer.end(), value.begin(), value.end());
            return *this;
        }

        Serializer& Write(const std::vector<uint8_t>& value)
        {
            Write(static_cast<int32_t>(value.size()));
            _buffer.insert(_buffer.end(), value.begin(), value.end());
            return *this;
        }

        template<typename T>
        Serializer& Write(const std::vector<T>& list, std::function<void(const T&)> writeItem)
        {
            Write(static_cast<int32_t>(list.size()));
            for (const T& item: list)
                writeItem(item);
            return *this;
        }

        std::vector<uint8_t> GetBytes() const
        {
            return _buffer;
        }
    };

    /// Reads a serialized buffer. By default skips the 2-byte `PacketType` header — see
    /// `NetworkProtocol::HeaderSize` and [Serializer.md](../../docs/classes/Serializer.md).
    ///
    /// Arithmetic reads use native endianness; cross-endian peers require explicit conversion.
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
        explicit Deserializer(const std::vector<uint8_t>& buf, size_t startOffset = NetworkProtocol::HeaderSize)
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

        std::string ReadString()
        {
            uint32_t len = ReadUInt();
            if (_offset + len > _buffer.size())
                throw std::out_of_range("Deserializer: string read past end of buffer");
            std::string str(reinterpret_cast<const char*>(&_buffer[_offset]), len);
            _offset += len;
            return str;
        }

        std::vector<uint8_t> ReadBytes()
        {
            int32_t count = ReadInt();
            if (count < 0 || _offset + count > _buffer.size())
                throw std::out_of_range("Deserializer: byte array read past end of buffer");
            std::vector<uint8_t> bytes(&_buffer[_offset], &_buffer[_offset] + count);
            _offset += count;
            return bytes;
        }

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
