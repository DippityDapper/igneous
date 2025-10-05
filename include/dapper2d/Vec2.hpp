#pragma once

#include <cmath>
#include <type_traits>

namespace Engine
{
    template<typename T>
    struct Vec2
    {
        static_assert(std::is_arithmetic_v<T>, "Vec2<T> requires a numeric type");

        T x = 0;
        T y = 0;

        Vec2() = default;

        Vec2(T _x, T _y)
                : x(_x), y(_y) {}

        template<typename U>
        explicit Vec2(const Vec2<U>& other)
                : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

        Vec2 operator*(T rhs) const
        {
            return { x * rhs, y * rhs };
        }

        Vec2 operator*(Vec2 rhs) const
        {
            return { x * rhs.x, y * rhs.y };
        }

        Vec2 operator/(T rhs) const
        {
            return { x / rhs, y / rhs };
        }

        Vec2 operator/(Vec2 rhs) const
        {
            return { x / rhs.x, y / rhs.y };
        }

        Vec2& operator+=(const Vec2& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        Vec2 operator+(const Vec2& rhs) const
        {
            return { x + rhs.x, y + rhs.y };
        }

        Vec2 operator-(const Vec2& rhs) const
        {
            return { x - rhs.x, y - rhs.y };
        }

        bool operator==(const Vec2& rhs) const
        {
            return x == rhs.x && y == rhs.y;
        }

        bool operator<(const Vec2& rhs) const
        {
            return (x < rhs.x) || (x == rhs.x && y < rhs.y);
        }

        Vec2<float> Normalized() const
        {
            float fx = static_cast<float>(x);
            float fy = static_cast<float>(y);
            float magnitude = std::sqrt(fx * fx + fy * fy);

            if (magnitude == 0.0f)
                return {0.0f, 0.0f};

            return { fx / magnitude, fy / magnitude };
        }

        float DistanceTo(const Vec2& to) const
        {
            float dx = static_cast<float>(x) - static_cast<float>(to.x);
            float dy = static_cast<float>(y) - static_cast<float>(to.y);
            return std::sqrt(dx * dx + dy * dy);
        }
    };
}
