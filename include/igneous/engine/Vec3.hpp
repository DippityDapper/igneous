#pragma once

#include <cmath>
#include <type_traits>

namespace Engine
{

    template<typename T>
    struct Vec3
    {
        static_assert(std::is_arithmetic_v<T>, "Vec3<T> requires a numeric type");

        T x = 0;

        T y = 0;

        T z = 0;

        Vec3() = default;

        Vec3(T _x, T _y, T _z)
            : x(_x), y(_y), z(_z)
        {
        }

        template<typename U>
        explicit Vec3(const Vec3<U>& other)
            : x(static_cast<T>(other.x)),
              y(static_cast<T>(other.y)),
              z(static_cast<T>(other.z))
        {
        }

        Vec3 operator*(T rhs) const
        {
            return {x * rhs, y * rhs, z * rhs};
        }

        Vec3 operator*(Vec3 rhs) const
        {
            return {x * rhs.x, y * rhs.y, z * rhs.z};
        }

        Vec3 operator/(T rhs) const
        {
            return {x / rhs, y / rhs, z / rhs};
        }

        Vec3 operator/(Vec3 rhs) const
        {
            return {x / rhs.x, y / rhs.y, z / rhs.z};
        }

        Vec3 operator%(int mod) const
        {
            return {x % mod, y % mod, z % mod};
        }

        Vec3& operator+=(const Vec3& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        Vec3 operator+(const Vec3& rhs) const
        {
            return {x + rhs.x, y + rhs.y, z + rhs.z};
        }

        Vec3 operator-(const Vec3& rhs) const
        {
            return {x - rhs.x, y - rhs.y, z - rhs.z};
        }

        bool operator==(const Vec3& rhs) const
        {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        bool operator!=(const Vec3& rhs) const
        {
            return x != rhs.x || y != rhs.y || z != rhs.z;
        }

        bool operator<(const Vec3& rhs) const
        {
            return (x < rhs.x) ||
                   (x == rhs.x && y < rhs.y) ||
                   (x == rhs.x && y == rhs.y && z < rhs.z);
        }

        bool operator>(const Vec3& rhs) const
        {
            return (x > rhs.x) ||
                   (x == rhs.x && y > rhs.y) ||
                   (x == rhs.x && y == rhs.y && z > rhs.z);
        }

        Vec3<float> Normalized() const
        {
            float fx = static_cast<float>(x);
            float fy = static_cast<float>(y);
            float fz = static_cast<float>(z);
            float magnitude = Magnitude();

            if (magnitude == 0.0f)
                return {0.0f, 0.0f, 0.0f};

            return {fx / magnitude, fy / magnitude, fz / magnitude};
        }

        float DistanceTo(const Vec3& to) const
        {
            float dx = static_cast<float>(x) - static_cast<float>(to.x);
            float dy = static_cast<float>(y) - static_cast<float>(to.y);
            float dz = static_cast<float>(z) - static_cast<float>(to.z);
            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }

        Vec3<int> Ceil() const
        {
            int dx = static_cast<int>(std::ceil(x));
            int dy = static_cast<int>(std::ceil(y));
            int dz = static_cast<int>(std::ceil(z));
            return {dx, dy, dz};
        }

        Vec3<int> Floor() const
        {
            int dx = static_cast<int>(std::floor(x));
            int dy = static_cast<int>(std::floor(y));
            int dz = static_cast<int>(std::floor(z));
            return {dx, dy, dz};
        }

        float Dot(const Vec3& rhs) const
        {
            const auto dx = static_cast<double>(x) * static_cast<double>(rhs.x);
            const auto dy = static_cast<double>(y) * static_cast<double>(rhs.y);
            const auto dz = static_cast<double>(z) * static_cast<double>(rhs.z);
            return static_cast<float>(dx + dy + dz);
        }

        float Magnitude() const
        {
            const auto dx = static_cast<double>(x) * static_cast<double>(x);
            const auto dy = static_cast<double>(y) * static_cast<double>(y);
            const auto dz = static_cast<double>(z) * static_cast<double>(z);
            return static_cast<float>(std::sqrt(dx + dy + dz));
        }

        Vec3<float> Cross(const Vec3& rhs) const
        {
            return {
                    static_cast<float>(static_cast<double>(y) * rhs.z - static_cast<double>(rhs.y) * z),
                    static_cast<float>(-(static_cast<double>(x) * rhs.z - static_cast<double>(rhs.x) * z)),
                    static_cast<float>(static_cast<double>(x) * rhs.y - static_cast<double>(y) * rhs.x)};
        }

        float Comp(const Vec3& onto) const
        {
            float dot = onto.Dot({x, y, z});
            float magSqr = std::pow(onto.Magnitude(), 2);
            return dot / magSqr;
        }

        Vec3<float> Proj(const Vec3& onto) const
        {
            return onto * Comp(onto);
        }
    };
}
