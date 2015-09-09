#ifndef SKETCH_3D_VECTOR_4_H
#define SKETCH_3D_VECTOR_4_H

#include "system/Common.h"

#include "math/Vector3.h"

namespace Sketch3D {

// Forward dependencies
class Matrix4x4;

/**
 * @class Vector4
 * The Vector4 class represents a 4D vector.
 */
class SKETCH_3D_API Vector4 {
    public:
        float                   x;      /**< X component */
        float                   y;      /**< Y component */
        float                   z;      /**< Z component */
        float                   w;      /**< Homogeneous component */

        // USEFUL STATICS

        static const Vector4    ZERO;   /**< Zero vector */
        static const Vector4    ONE;    /**< One vector */
        static const Vector4    RIGHT;  /**< Only the x component is set to 1 */
        static const Vector4    UP;     /**< Only the y component is set to 1 */
        static const Vector4    LOOK;   /**< Only the z component is set to 1 */

        /**
         * Default constructor. Initializes everything to 0.
         */
                                Vector4();

        /**
         * Constructor. The w component is set to 1
         * @param x The x component
         * @param y The y component
         * @param z The z component
         */
                                Vector4(float x, float y, float z);

        /**
         * Constructor
         * @param x The x component
         * @param y The y component
         * @param z The z component
         * @param w The w component
         */
                                Vector4(float x, float y, float z, float w);

        /**
         * Copy constructor
         * @param src The src vector that will be copied
         */
                                Vector4(const Vector4& src);

        /**
         * Returns the length of the vector. It should be noted that the length
         * is a costly operation since it requires a square root. You may want
         * to check the SquaredLength function.
         */
        float                   Length() const;

        /**
         * Returns the squared length of the vector.
         */
        float                   SquaredLength() const;

        /**
         * Return the normalized vector
         */
        Vector4                 Normalized() const;

        /**
         * INLINE normalization of the vector
         */
        void                    Normalize();
    
        /**
         * Computes the dot product between this vector and v.
         * @param v The vector v with which the dot product will be computed
         * @return the dot product
         */
        float                   Dot(const Vector4& v) const;

        /**
         * Computes the cross product between this vector and v.
         * @param v The vector v with which to compute the cross product
         * @return a new vector perpendicular to this one and v
         */
        Vector4                 Cross(const Vector4& v) const;

        // UNARY OPERATORS
        /**
         * Returns the negated vector
         */
        INLINE Vector4          operator-() const;

        // BINARY OPERATORS
        INLINE Vector4          operator+(float f) const;
        INLINE Vector4          operator+(const Vector4& v) const;
        INLINE Vector4          operator-(float f) const;
        INLINE Vector4          operator-(const Vector4& v) const;
        INLINE Vector4          operator*(float f) const;
        Vector4					operator*(const Matrix4x4& m) const;
        INLINE Vector4          operator/(float f) const;

        INLINE friend Vector4   operator+(float f, const Vector4& v);
        INLINE friend Vector4   operator-(float f, const Vector4& v);
        INLINE friend Vector4   operator*(float f, const Vector4& v);

        INLINE void             operator+=(float f);
        INLINE void             operator+=(const Vector4& v);
        INLINE void             operator-=(float f);
        INLINE void             operator-=(const Vector4& v);
        INLINE void             operator*=(float f);
        void					operator*=(const Matrix4x4& m);
        INLINE void             operator/=(float f);

        INLINE bool             operator==(const Vector4& v) const;
        INLINE bool             operator!=(const Vector4& v) const;

        INLINE Vector4&         operator=(const Vector4& v);
        INLINE Vector4&         operator=(const Vector3& v);

};

INLINE Vector4 Vector4::operator-() const
{
    return Vector4(-x, -y, -z);
}

INLINE Vector4 Vector4::operator+(float f) const
{
    return Vector4(x + f, y + f, z + f);
}

INLINE Vector4 Vector4::operator+(const Vector4& v) const
{
    return Vector4(x + v.x, y + v.y, z + v.z);
}

INLINE Vector4 Vector4::operator-(float f) const
{
    return Vector4(x - f, y - f, z - f);
}

INLINE Vector4 Vector4::operator-(const Vector4& v) const
{
    return Vector4(x - v.x, y - v.y, z - v.z);
}

INLINE Vector4 Vector4::operator*(float f) const
{
    return Vector4(x * f, y * f, z * f);
}

INLINE Vector4 Vector4::operator/(float f) const
{
    return Vector4(x / f, y / f, z / f);
}

INLINE Vector4 operator+(float f, const Vector4& v)
{
    return Vector4(v.x + f, v.y + f, v.z + f);
}

INLINE Vector4 operator-(float f, const Vector4& v)
{
    return Vector4(v.x - f, v.y - f, v.z - f);
}

INLINE Vector4 operator*(float f, const Vector4& v)
{
    return Vector4(v.x * f, v.y * f, v.z * f);
}

INLINE void Vector4::operator+=(float f)
{
    x += f;
    y += f;
    z += f;
}

INLINE void Vector4::operator+=(const Vector4& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
}

INLINE void Vector4::operator-=(float f)
{
    x -= f;
    y -= f;
    z -= f;
}

INLINE void Vector4::operator-=(const Vector4& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
}

INLINE void Vector4::operator*=(float f)
{
    x *= f;
    y *= f;
    z *= f;
}

INLINE void Vector4::operator/=(float f)
{
    x /= f;
    y /= f;
    z /= f;
}

INLINE bool Vector4::operator==(const Vector4& v) const
{
    return fabs(x - v.x) < EPSILON &&
           fabs(y - v.y) < EPSILON &&
           fabs(z - v.z) < EPSILON;
}

INLINE bool Vector4::operator!=(const Vector4& v) const
{
    return fabs(x - v.x) >= EPSILON ||
           fabs(y - v.y) >= EPSILON ||
           fabs(z - v.z) >= EPSILON;
}

INLINE Vector4& Vector4::operator=(const Vector4& v)
{
    if ((*this) == v) {
        return *this;
    }

    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;

    return *this;
}

INLINE Vector4& Vector4::operator=(const Vector3& v)
{
    x = v.x;
    y = v.y;
    z = v.z;

    return *this;
}

}

#endif

