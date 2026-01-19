#pragma once

#include <cmath>
#include <type_traits>

namespace Engine
{
    /**
     * @class Vec3
     * @brief A generic 3D vector type.
     *
     * Vec3 represents a three-dimensional mathematical vector with components
     * of an arithmetic type. It provides common vector operations including
     * arithmetic operators, normalization, dot product, cross product,
     * projection, and distance calculations.
     *
     * This type is suitable for representing positions, directions, velocities,
     * and other 3D quantities in engine and gameplay code.
     *
     * @tparam T An arithmetic type (int, float, double, etc.).
     *
     * @note Vec3<T> requires T to be an arithmetic type.
     */
    template<typename T>
    struct Vec3
    {
        static_assert(std::is_arithmetic_v<T>, "Vec3<T> requires a numeric type");

        /**
         * @brief X component of the vector.
         */
        T x = 0;

        /**
         * @brief Y component of the vector.
         */
        T y = 0;

        /**
         * @brief Z component of the vector.
         */
        T z = 0;

        /**
         * @brief Default constructor.
         *
         * Initializes the vector to (0, 0, 0).
         */
        Vec3() = default;

        /**
         * @brief Constructs a vector with the given components.
         *
         * @param _x X component.
         * @param _y Y component.
         * @param _z Z component.
         */
        Vec3(T _x, T _y, T _z)
            : x(_x), y(_y), z(_z)
        {
        }

        /**
         * @brief Explicit conversion constructor from another Vec3 type.
         *
         * Converts the components of another Vec3<U> to type T.
         *
         * @tparam U Source vector component type.
         * @param other The vector to convert from.
         */
        template<typename U>
        explicit Vec3(const Vec3<U>& other)
            : x(static_cast<T>(other.x)),
              y(static_cast<T>(other.y)),
              z(static_cast<T>(other.z))
        {
        }

        /**
         * @brief Scalar multiplication.
         *
         * @param rhs Scalar to multiply by.
         * @return A new vector scaled by the scalar.
         */
        Vec3 operator*(T rhs) const
        {
            return {x * rhs, y * rhs, z * rhs};
        }

        /**
         * @brief Component-wise vector multiplication.
         *
         * @param rhs Vector to multiply by.
         * @return A new vector with component-wise multiplication applied.
         */
        Vec3 operator*(Vec3 rhs) const
        {
            return {x * rhs.x, y * rhs.y, z * rhs.z};
        }

        /**
         * @brief Scalar division.
         *
         * @param rhs Scalar to divide by.
         * @return A new vector divided by the scalar.
         *
         * @note No division-by-zero checks are performed.
         */
        Vec3 operator/(T rhs) const
        {
            return {x / rhs, y / rhs, z / rhs};
        }

        /**
         * @brief Component-wise vector division.
         *
         * @param rhs Vector to divide by.
         * @return A new vector with component-wise division applied.
         *
         * @note No division-by-zero checks are performed.
         */
        Vec3 operator/(Vec3 rhs) const
        {
            return {x / rhs.x, y / rhs.y, z / rhs.z};
        }

        /**
         * @brief Component-wise modulo operation.
         *
         * @param mod Modulo value.
         * @return A new vector with each component modulo `mod`.
         *
         * @note Intended for integral vector types.
         */
        Vec3 operator%(int mod) const
        {
            return {x % mod, y % mod, z % mod};
        }

        /**
         * @brief Adds another vector to this one.
         *
         * @param rhs Vector to add.
         * @return Reference to this vector after modification.
         */
        Vec3& operator+=(const Vec3& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        /**
         * @brief Vector addition.
         *
         * @param rhs Vector to add.
         * @return The resulting vector.
         */
        Vec3 operator+(const Vec3& rhs) const
        {
            return {x + rhs.x, y + rhs.y, z + rhs.z};
        }

        /**
         * @brief Vector subtraction.
         *
         * @param rhs Vector to subtract.
         * @return The resulting vector.
         */
        Vec3 operator-(const Vec3& rhs) const
        {
            return {x - rhs.x, y - rhs.y, z - rhs.z};
        }

        /**
         * @brief Equality comparison.
         *
         * @param rhs Vector to compare against.
         * @return true if all components are equal.
         */
        bool operator==(const Vec3& rhs) const
        {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        /**
         * @brief Inequality comparison against a float vector.
         *
         * @param rhs Vector to compare against.
         * @return true if any component differs.
         */
        bool operator!=(const Vec3<float>& rhs) const
        {
            return x != rhs.x || y != rhs.y || z != rhs.z;
        }

        /**
         * @brief Lexicographical less-than comparison.
         *
         * Compares x first, then y, then z.
         *
         * @param rhs Vector to compare against.
         * @return true if this vector is less than rhs.
         */
        bool operator<(const Vec3& rhs) const
        {
            return (x < rhs.x) ||
                   (x == rhs.x && y < rhs.y) ||
                   (x == rhs.x && y == rhs.y && z < rhs.z);
        }

        /**
         * @brief Lexicographical greater-than comparison.
         *
         * @param rhs Vector to compare against.
         * @return true if this vector is greater than rhs.
         */
        bool operator>(const Vec3& rhs) const
        {
            return (x > rhs.x) ||
                   (x == rhs.x && y > rhs.y) ||
                   (x == rhs.x && y == rhs.y && z > rhs.z);
        }

        /**
         * @brief Returns a normalized copy of the vector.
         *
         * @return A unit-length vector pointing in the same direction.
         *         Returns (0, 0, 0) if the magnitude is zero.
         */
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

        /**
         * @brief Computes the distance to another vector.
         *
         * @param to Target vector.
         * @return Euclidean distance between the two vectors.
         */
        float DistanceTo(const Vec3& to) const
        {
            float dx = static_cast<float>(x) - static_cast<float>(to.x);
            float dy = static_cast<float>(y) - static_cast<float>(to.y);
            float dz = static_cast<float>(z) - static_cast<float>(to.z);
            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }

        /**
         * @brief Returns a vector with components rounded up.
         *
         * @return A Vec3<int> with ceiling applied to each component.
         */
        Vec3<int> Ceil() const
        {
            int dx = (int) std::ceil(x);
            int dy = (int) std::ceil(y);
            int dz = (int) std::ceil(z);
            return {dx, dy, dz};
        }

        /**
         * @brief Returns a vector with components rounded down.
         *
         * @return A Vec3<int> with floor applied to each component.
         */
        Vec3<int> Floor() const
        {
            int dx = (int) std::floor(x);
            int dy = (int) std::floor(y);
            int dz = (int) std::floor(z);
            return {dx, dy, dz};
        }

        /**
         * @brief Computes the dot product with another vector.
         *
         * @param rhs Vector to dot with.
         * @return Dot product of the two vectors.
         */
        float Dot(const Vec3& rhs) const
        {
            int dx = x * rhs.x;
            int dy = y * rhs.y;
            int dz = z * rhs.z;
            return dx + dy + dz;
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
            int dz = std::pow(z, 2);
            return std::sqrt(dx + dy + dz);
        }

        /**
         * @brief Computes the cross product with another vector.
         *
         * @param rhs Vector to cross with.
         * @return A vector perpendicular to both input vectors.
         */
        Vec3<float> Cross(const Vec3& rhs) const
        {
            return {
                    (y * rhs.z - rhs.y * z),
                    -(x * rhs.z - rhs.x * z),
                    (x * rhs.y - y * rhs.x)};
        }

        /**
         * @brief Computes the scalar component of this vector onto another.
         *
         * @param onto Vector to project onto.
         * @return Scalar projection value.
         */
        float Comp(const Vec3& onto) const
        {
            float dot = onto.Dot({x, y, z});
            float magSqr = std::pow(onto.Magnitude(), 2);
            return dot / magSqr;
        }

        /**
         * @brief Projects this vector onto another vector.
         *
         * @param onto Vector to project onto.
         * @return The projected vector.
         */
        Vec3<float> Proj(const Vec3& onto) const
        {
            return onto * Comp(onto);
        }
    };
}
