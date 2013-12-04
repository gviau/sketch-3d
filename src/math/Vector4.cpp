#include "Vector4.h"

#include <math.h>

#include "Constants.h"
#include "Matrix4x4.h"
#include "Platform.h"

#include <iostream>

namespace Sketch3D {

const Vector4 Vector4::ZERO (0.0f, 0.0f, 0.0f);
const Vector4 Vector4::ONE  (1.0f, 1.0f, 1.0f);
const Vector4 Vector4::RIGHT(1.0f, 0.0f, 0.0f);
const Vector4 Vector4::UP   (0.0f, 1.0f, 0.0f);
const Vector4 Vector4::LOOK (0.0f, 0.0f, 1.0f);

Vector4::Vector4() : x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{
}

Vector4::Vector4(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
    w = 1.0f;
}

Vector4::Vector4(const Vector4& src)
{
    x = src.x;
    y = src.y;
    z = src.z;
    w = 1.0f;
}

float Vector4::Length() const
{
    float f;
    if (!PlatformInformation::HasCpuFeature(PlatformInformation::SSE2)) {
        f = sqrtf(x*x + y*y + z*z);
    } else {
        __m128 u = {x, y, z, 0.0f};
        __m128 v;

        u = _mm_mul_ps(u, u);
        v = u;
        v = _mm_shuffle_ps(v, v, 0x0000004e);
        u = _mm_add_ps(u, v);
        v = u;
        v = _mm_shuffle_ps(v, v, 0x00000011);
        u = _mm_add_ps(u, v);
        u = _mm_sqrt_ss(u);
        _mm_store_ss(&f, u);
    }

    return f;
}

float Vector4::SquaredLength() const
{
    return x*x + y*y + z*z;
}

Vector4 Vector4::Normalized() const
{
    Vector4 result;
    float length;
    if (!PlatformInformation::HasCpuFeature(PlatformInformation::SSE2)) {
        length = Length();
        if (length != 0.0f) {
            result.x /= length;
            result.y /= length;
            result.z /= length;
        }
    } else {
        __m128 u = {x, y, z, 0.0f};
        __m128 v;
        __m128 w = u;

        u = _mm_mul_ps(u, u);
        v = u;
        v = _mm_shuffle_ps(v, v, 0x0000004e);
        u = _mm_add_ps(u, v);
        v = u;
        v = _mm_shuffle_ps(v, v, 0x00000011);
        u = _mm_add_ps(u, v);

        u = _mm_rsqrt_ps(u);
        w = _mm_mul_ps(u, w);

        float f[4];
        _mm_store_ps(f, w);

        result.x = f[0];
        result.y = f[1];
        result.z = f[2];
    }

    return result;
}

void Vector4::Normalize()
{
    float length;
    if (!PlatformInformation::HasCpuFeature(PlatformInformation::SSE2)) {
        length = Length();
        if (length != 0.0f) {
            x /= length;
            y /= length;
            z /= length;
        }
    } else {
        __m128 u = {x, y, z, 0.0f};
        __m128 v;
        __m128 w = u;

        u = _mm_mul_ps(u, u);
        v = u;
        v = _mm_shuffle_ps(v, v, 0x0000004e);
        u = _mm_add_ps(u, v);
        v = u;
        v = _mm_shuffle_ps(v, v, 0x00000011);
        u = _mm_add_ps(u, v);

        u = _mm_rsqrt_ps(u);
        w = _mm_mul_ps(u, w);

        float f[4];
        _mm_store_ps(f, w);

        x = f[0];
        y = f[1];
        z = f[2];
    }
}

float Vector4::Dot(const Vector4& v) const
{
    return x*v.x + y*v.y + z*v.z;
}

Vector4 Vector4::Cross(const Vector4& v) const
{
    return Vector4(y*v.z - z*v.y,
                   z*v.x - x*v.z,
                   x*v.y - y*v.x);
}

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

INLINE Vector4 Vector4::operator*(const Matrix4x4& m) const
{
    // We do the product the same way as if we were multiplying M * v, since
    // it makes more since for linear transformation to do it that way.
    Vector4 v;

    v.x = x * m(0, 0) + y * m(1, 0) + z * m(2, 0) + m(3, 0);
    v.y = x * m(0, 1) + y * m(1, 1) + z * m(2, 1) + m(3, 1);
    v.z = x * m(0, 2) + y * m(1, 2) + z * m(2, 2) + m(3, 2);
    v.w = x * m(0, 3) + y * m(1, 3) + z * m(2, 3) + m(3, 3);

    v.x /= v.w;
    v.y /= v.w;
    v.z /= v.w;
    v.w = 1.0f;

    return v;
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

INLINE void Vector4::operator*=(const Matrix4x4& m)
{
    // We do the product the same way as if we were multiplying M * v, since
    // it makes more since for linear transformation to do it that way.
    float nx, ny, nz;

    nx = x * m(0, 0) + y * m(1, 0) + z * m(2, 0) + m(3, 0);
    ny = x * m(0, 1) + y * m(1, 1) + z * m(2, 1) + m(3, 1);
    nz = x * m(0, 2) + y * m(1, 2) + z * m(2, 2) + m(3, 2);
    w  = x * m(0, 3) + y * m(1, 3) + z * m(2, 3) + m(3, 3);

    x = nx / w;
    y = ny / w;
    z = nz / w;
    w = 1.0f;
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

    return *this;
}

}

