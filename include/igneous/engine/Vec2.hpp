#pragma once

#include <cmath>
#include <type_traits>

namespace Engine
{
    /**
     * @class Vec2
     * @brief A generic 2D vector type.
     *
     * Vec2 represents a two-dimensional mathematical vector with components
     * of an arithmetic type. It provides common vector operations such as
     * addition, subtraction, scaling, normalization, dot product, cross
     * product, and projection.
     *
     * This class is designed to be lightweight and usable with different
     * numeric types (e.g., int, float, double).
     *
     * @tparam T An arithmetic type (int, float, double, etc.).
     *
     * @note Vec2<T> requires T to be an arithmetic type.
     */
    template<typename T>
    struct Vec2
    {
        static_assert(std::is_arithmetic_v<T>, "Vec2<T> requires a numeric type");

        /**
         * @brief X component of the vector.
         */
        T x = 0;

        /**
         * @brief Y component of the vector.
         */
        T y = 0;

        /**
         * @brief Default constructor.
         *
         * Initializes the vector to (0, 0).
         */
        Vec2() = default;

        /**
         * @brief Constructs a vector with the given components.
         *
         * @param _x X component.
         * @param _y Y component.
         */
        Vec2(T _x, T _y)
            : x(_x), y(_y)
        {
        }

        /**
         * @brief Explicit conversion constructor from another Vec2 type.
         *
         * Converts the components of another Vec2<U> to type T.
         *
         * @tparam U Source vector component type.
         * @param other The vector to convert from.
         */
        template<typename U>
        explicit Vec2(const Vec2<U>& other)
            : x(static_cast<T>(other.x)), y(static_cast<T>(other.y))
        {
        }

        /**
         * @brief Scalar multiplication.
         *
         * @param rhs Scalar to multiply by.
         * @return A new vector scaled by the scalar.
         */
        Vec2 operator*(T rhs) const
        {
            return {x * rhs, y * rhs};
        }

        /**
         * @brief Component-wise vector multiplication.
         *
         * @param rhs Vector to multiply by.
         * @return A new vector with component-wise multiplication applied.
         */
        Vec2 operator*(Vec2 rhs) const
        {
            return {x * rhs.x, y * rhs.y};
        }

        /**
         * @brief Scalar division.
         *
         * @param rhs Scalar to divide by.
         * @return A new vector divided by the scalar.
         *
         * @note No division-by-zero checks are performed.
         */
        Vec2 operator/(T rhs) const
        {
            return {x / rhs, y / rhs};
        }

        /**
         * @brief Component-wise vector division.
         *
         * @param rhs Vector to divide by.
         * @return A new vector with component-wise division applied.
         *
         * @note No division-by-zero checks are performed.
         */
        Vec2 operator/(Vec2 rhs) const
        {
            return {x / rhs.x, y / rhs.y};
        }

        /**
         * @brief Component-wise modulo operation.
         *
         * @param mod Modulo value.
         * @return A new vector with each component modulo `mod`.
         *
         * @note Intended for integral vector types.
         */
        Vec2 operator%(int mod) const
        {
            return {x % mod, y % mod};
        }

        /**
         * @brief Adds another vector to this one.
         *
         * @param rhs Vector to add.
         * @return Reference to this vector after modification.
         */
        Vec2& operator+=(const Vec2& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        /**
         * @brief Vector addition.
         *
         * @param rhs Vector to add.
         * @return The resulting vector.
         */
        Vec2 operator+(const Vec2& rhs) const
        {
            return {x + rhs.x, y + rhs.y};
        }

        /**
         * @brief Vector subtraction.
         *
         * @param rhs Vector to subtract.
         * @return The resulting vector.
         */
        Vec2 operator-(const Vec2& rhs) const
        {
            return {x - rhs.x, y - rhs.y};
        }

        /**
         * @brief Equality comparison.
         *
         * @param rhs Vector to compare against.
         * @return true if both components are equal.
         */
        bool operator==(const Vec2& rhs) const
        {
            return x == rhs.x && y == rhs.y;
        }

        /**
         * @brief Inequality comparison against a float vector.
         *
         * @param vec2 Vector to compare against.
         * @return true if either component differs.
         */
        bool operator!=(const Vec2<float>& vec2) const
        {
            return x != vec2.x || y != vec2.y;
        }

        /**
         * @brief Lexicographical less-than comparison.
         *
         * Compares x first, then y if x values are equal.
         *
         * @param rhs Vector to compare against.
         * @return true if this vector is less than rhs.
         */
        bool operator<(const Vec2& rhs) const
        {
            return (x < rhs.x) || (x == rhs.x && y < rhs.y);
        }

        /**
         * @brief Lexicographical greater-than comparison.
         *
         * @param rhs Vector to compare against.
         * @return true if this vector is greater than rhs.
         */
        bool operator>(const Vec2& rhs) const
        {
            return (x > rhs.x) || (x == rhs.x && y > rhs.y);
        }

        /**
         * @brief Returns a normalized copy of the vector.
         *
         * @return A unit-length vector pointing in the same direction.
         *         Returns (0, 0) if the magnitude is zero.
         */
        Vec2<float> Normalized() const
        {
            float fx = static_cast<float>(x);
            float fy = static_cast<float>(y);
            float magnitude = std::sqrt(fx * fx + fy * fy);

            if (magnitude == 0.0f)
                return {0.0f, 0.0f};

            return {fx / magnitude, fy / magnitude};
        }

        /**
         * @brief Computes the distance to another vector.
         *
         * @param to Target vector.
         * @return Euclidean distance between the two vectors.
         */
        float DistanceTo(const Vec2& to) const
        {
            float dx = static_cast<float>(x) - static_cast<float>(to.x);
            float dy = static_cast<float>(y) - static_cast<float>(to.y);
            return std::sqrt(dx * dx + dy * dy);
        }

        /**
         * @brief Returns a vector with components rounded up.
         *
         * @return A Vec2<int> with ceiling applied to each component.
         */
        Vec2<int> Ceil() const
        {
            int dx = (int) std::ceil(x);
            int dy = (int) std::ceil(y);
            return {dx, dy};
        }

        /**
         * @brief Returns a vector with components rounded down.
         *
         * @return A Vec2<int> with floor applied to each component.
         */
        Vec2<int> Floor() const
        {
            int dx = (int) std::floor(x);
            int dy = (int) std::floor(y);
            return {dx, dy};
        }

        /**
         * @brief Computes the dot product with another vector.
         *
         * @param rhs Vector to dot with.
         * @return Dot product of the two vectors.
         */
        float Dot(const Vec2& rhs) const
        {
            int dx = x * rhs.x;
            int dy = y * rhs.y;
            return dx + dy;
        }

        /**
         * @brief Computes the magnitude (length) of the vector.
         *
         * @return Length of the vector.
         */
        float Magnitude() const
        {
            int dx = std::pow(x, 2);
            int dy = std::pow(y, 2);
            return std::sqrt(dx + dy);
        }

        /**
         * @brief Computes the 2D cross product (scalar result).
         *
         * @param rhs Vector to cross with.
         * @return Scalar cross product value.
         */
        float Cross(const Vec2& rhs) const
        {
            return x * rhs.y - rhs.x * y;
        }

        /**
         * @brief Computes the scalar component of this vector onto another.
         *
         * @param onto Vector to project onto.
         * @return Scalar projection value.
         */
        float Comp(const Vec2& onto) const
        {
            float dot = onto.Dot({x, y});
            float magSqr = std::pow(onto.Magnitude(), 2);
            return dot / magSqr;
        }

        /**
         * @brief Projects this vector onto another vector.
         *
         * @param onto Vector to project onto.
         * @return The projected vector.
         */
        Vec2<float> Proj(const Vec2& onto) const
        {
            return onto * Comp(onto);
        }
    };
}
