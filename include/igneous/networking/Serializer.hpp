#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <cstring>

namespace Engine
{
    class Serializer
    {
    private:
        std::vector<uint8_t>& buffer;

    public:
        explicit Serializer(std::vector<uint8_t>& buf) : buffer(buf) {}

        template<typename T>
        requires std::is_arithmetic_v<T>
        Serializer& operator<<(const T& value)
        {
            const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
            buffer.insert(buffer.end(), bytes, bytes + sizeof(T));
            return *this;
        }

        Serializer& operator<<(const std::string& str)
        {
            uint32_t len = str.size();
            *this << len;
            buffer.insert(buffer.end(), str.begin(), str.end());
            return *this;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        Serializer& operator<<(const std::vector<T>& vec)
        {
            uint32_t count = vec.size();
            *this << count;
            for (const auto& item : vec)
                *this << item;
            return *this;
        }
    };

    class Deserializer
    {
    private:
        const std::vector<uint8_t>& buffer;
        size_t offset;

    public:
        explicit Deserializer(const std::vector<uint8_t>& buf, size_t startOffset = 1)
            : buffer(buf), offset(startOffset) {}

        template<typename T>
        requires std::is_arithmetic_v<T>
        Deserializer& operator>>(T& value)
        {
            std::memcpy(&value, &buffer[offset], sizeof(T));
            offset += sizeof(T);
            return *this;
        }

        Deserializer& operator>>(std::string& str)
        {
            uint32_t len;
            *this >> len;
            str.assign(reinterpret_cast<const char*>(&buffer[offset]), len);
            offset += len;
            return *this;
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        Deserializer& operator>>(std::vector<T>& vec)
        {
            uint32_t count;
            *this >> count;
            vec.resize(count);
            for (auto& item : vec)
                *this >> item;
            return *this;
        }

        size_t GetOffset() const { return offset; }
    };
}
