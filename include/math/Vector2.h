#ifndef SKETCH_3D_VECTOR_2
#define SKETCH_3D_VECTOR_2

#include "system/Common.h"

namespace Sketch3D {

/**
 * @class Vector2
 * Represents a 2D vector
 */
class Vector2 {
    public:
        float                   x;  /**< X component */
        float                   y;  /**< Y component */

        // USEFUL STATICS
        static const Vector2    ZERO;
        static const Vector2    RIGHT;
        static const Vector2    UP;
        static const Vector2    ONE;

        /**
         * Default constructor. Initializes everything to 0
         */
                                Vector2();

        /**
         * Constructor.
         * @param x The x component
         * @param y The y component
         */
                                Vector2(float x, float y);

        /**
         * Copy constructor
         * @src The src vector that will be copied
         */
                                Vector2(const Vector2& src);

        /**
         * Returns the length of the vector
         */
        float                   Length() const;

        /**
         * Returns the squared length of the vector
         */
        float                   SquaredLength() const;

        /**
         * Returns the normalized vector
         */
        Vector2                 Normalized() const;

        /**
         * Inline normalization
         */
        void                    Normalize();

        /**
         * Computes the dot product between this vector and another one
         * @param v The other vector with which to compute the dot vector
         */
        float                   Dot(const Vector2& v) const;

        /**
         * Compute the vector perpendicular to this one
         */
        Vector2                 Perpendicular() const;

        // UNARY OPERATORS
        /**
         * Returns the negated vector
         */
        INLINE Vector2          operator-() const;

        // BINARY OPERATORS
        INLINE Vector2          operator+(float f) const;
        INLINE Vector2          operator+(const Vector2& v) const;
        INLINE Vector2          operator-(float f) const;
        INLINE Vector2          operator-(const Vector2& v) const;
        INLINE Vector2          operator*(float f) const;
        INLINE Vector2          operator/(float f) const;

        INLINE friend Vector2   operator+(float f, const Vector2& v);
        INLINE friend Vector2   operator-(float f, const Vector2& v);
        INLINE friend Vector2   operator*(float f, const Vector2& v);

        INLINE void             operator+=(float f);
        INLINE void             operator+=(const Vector2& v);
        INLINE void             operator-=(float f);
        INLINE void             operator-=(const Vector2& v);
        INLINE void             operator*=(float f);
        INLINE void             operator/=(float f);

        INLINE bool             operator==(const Vector2& v) const;
        INLINE bool             operator!=(const Vector2& v) const;

        INLINE Vector2&         operator=(const Vector2& v);
};

INLINE Vector2 Vector2::operator-() const {
    return Vector2(-x, -y);
}

INLINE Vector2 Vector2::operator+(float f) const {
    return Vector2(x + f, y + f);
}

INLINE Vector2 Vector2::operator+(const Vector2& v) const {
    return Vector2(x + v.x, y + v.y);
}

INLINE Vector2 Vector2::operator-(float f) const {
    return Vector2(x - f, y - f);
}

INLINE Vector2 Vector2::operator-(const Vector2& v) const {
    return Vector2(x - v.x, y - v.y);
}

INLINE Vector2 Vector2::operator*(float f) const {
    return Vector2(x * f, y * f);
}

INLINE Vector2 Vector2::operator/(float f) const {
    return Vector2(x / f, y / f);
}

INLINE Vector2 operator+(float f, const Vector2& v) {
    return Vector2(v.x + f, v.y + f);
}

INLINE Vector2 operator-(float f, const Vector2& v) {
    return Vector2(v.x - f, v.y - f);
}

INLINE Vector2 operator*(float f, const Vector2& v) {
    return Vector2(v.x * f, v.y * f);
}

INLINE void Vector2::operator+=(float f) {
    x += f;
    y += f;
}

INLINE void Vector2::operator+=(const Vector2& v) {
    x += v.x;
    y += v.y;
}

INLINE void Vector2::operator-=(float f) {
    x -= f;
    y -= f;
}

INLINE void Vector2::operator-=(const Vector2& v) {
    x -= v.x;
    y -= v.y;
}

INLINE void Vector2::operator*=(float f) {
    x *= f;
    y *= f;
}

INLINE void Vector2::operator/=(float f) {
    x /= f;
    y /= f;
}

INLINE bool Vector2::operator==(const Vector2& v) const {
    return (fabs(x - v.x) < 0.00001f &&
            fabs(y - v.y) < 0.00001f);
}

INLINE bool Vector2::operator!=(const Vector2& v) const {
    return (fabs(x - v.x) >= 0.00001f ||
            fabs(y - v.y) >= 0.00001f);
}

INLINE Vector2& Vector2::operator=(const Vector2& v) {
    if (this != &v) {
        x = v.x;
        y = v.y;
    }

    return *this;
}

}

#endif