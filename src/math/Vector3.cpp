#include "Vector3.h"

#include "Matrix3x3.h"
#include "Constants.h"
#include <math.h>

namespace Sketch3D {

const Vector3 Vector3::ZERO (0.0f, 0.0f, 0.0f);
const Vector3 Vector3::ONE  (1.0f, 1.0f, 1.0f);
const Vector3 Vector3::RIGHT(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UP   (0.0f, 1.0f, 0.0f);
const Vector3 Vector3::LOOK (0.0f, 0.0f, 1.0f);

Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f)
{
}

Vector3::Vector3(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Vector3::Vector3(const Vector3& src)
{
    x = src.x;
    y = src.y;
    z = src.z;
}

float Vector3::Length() const
{
    return sqrtf(x*x + y*y + z*z);
}

float Vector3::SquaredLength() const
{
    return x*x + y*y + z*z;
}

Vector3 Vector3::Normalized() const
{
    float length = Length();
    return Vector3(x / length, y / length, z / length);
}

void Vector3::Normalize()
{
    float length = Length();
    x /= length;
    y /= length;
    z /= length;
}

float Vector3::Dot(const Vector3& v) const
{
    return x*v.x + y*v.y + z*v.z;
}

Vector3 Vector3::Cross(const Vector3& v) const
{
    return Vector3(y*v.z - z*v.y,
                   z*v.x - x*v.z,
                   x*v.y - y*v.x);
}

INLINE Vector3 Vector3::operator-() const
{
    return Vector3(-x, -y, -z);
}

INLINE Vector3 Vector3::operator+(float f) const
{
    return Vector3(x + f, y + f, z + f);
}

INLINE Vector3 Vector3::operator+(const Vector3& v) const
{
    return Vector3(x + v.x, y + v.y, z + v.z);
}

INLINE Vector3 Vector3::operator-(float f) const
{
    return Vector3(x - f, y - f, z - f);
}

INLINE Vector3 Vector3::operator-(const Vector3& v) const
{
    return Vector3(x - v.x, y - v.y, z - v.z);
}

INLINE Vector3 Vector3::operator*(float f) const
{
    return Vector3(x * f, y * f, z * f);
}

INLINE Vector3 Vector3::operator*(const Matrix3x3& m) const
{
    // We do the product the same way as if we were multiplying M * v, since
    // it makes more since for linear transformation to do it that way.
    Vector3 v;

    v.x = x * m(0, 0) + y * m(1, 0) + z * m(2, 0);
    v.y = x * m(0, 1) + y * m(1, 1) + z * m(2, 1);
    v.z = x * m(0, 2) + y * m(1, 2) + z * m(2, 2);

    return v;
}

INLINE Vector3 Vector3::operator/(float f) const
{
    return Vector3(x / f, y / f, z / f);
}

INLINE Vector3 operator+(float f, const Vector3& v)
{
    return Vector3(v.x + f, v.y + f, v.z + f);
}

INLINE Vector3 operator-(float f, const Vector3& v)
{
    return Vector3(v.x - f, v.y - f, v.z - f);
}

INLINE Vector3 operator*(float f, const Vector3& v)
{
    return Vector3(v.x * f, v.y * f, v.z * f);
}

INLINE void Vector3::operator+=(float f)
{
    x += f;
    y += f;
    z += f;
}

INLINE void Vector3::operator+=(const Vector3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
}

INLINE void Vector3::operator-=(float f)
{
    x -= f;
    y -= f;
    z -= f;
}

INLINE void Vector3::operator-=(const Vector3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
}

INLINE void Vector3::operator*=(float f)
{
    x *= f;
    y *= f;
    z *= f;
}

INLINE void Vector3::operator*=(const Matrix3x3& m)
{
    // We do the product the same way as if we were multiplying M * v, since
    // it makes more since for linear transformation to do it that way.
    float nx, ny, nz;

    nx = x * m(0, 0) + y * m(1, 0) + z * m(2, 0);
    ny = y * m(0, 1) + y * m(1, 1) + z * m(2, 1);
    nz = z * m(0, 2) + y * m(1, 2) + z * m(2, 2);

    x = nx;
    y = ny;
    z = nz;
}

INLINE void Vector3::operator/=(float f)
{
    x /= f;
    y /= f;
    z /= f;
}

INLINE bool Vector3::operator==(const Vector3& v) const
{
    return fabs(x - v.x) < EPSILON &&
           fabs(y - v.y) < EPSILON &&
           fabs(z - v.z) < EPSILON;
}

INLINE bool Vector3::operator!=(const Vector3& v) const
{
    return fabs(x - v.x) >= EPSILON ||
           fabs(y - v.y) >= EPSILON ||
           fabs(z - v.z) >= EPSILON;
}

INLINE Vector3& Vector3::operator=(const Vector3& v)
{
    if ((*this) == v) {
        return *this;
    }

    x = v.x;
    y = v.y;
    z = v.z;

    return *this;
}

}
