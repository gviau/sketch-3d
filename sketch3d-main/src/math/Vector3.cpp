#include "math/Vector3.h"

#include "math/Constants.h"
#include "math/Matrix3x3.h"
#include "math/Vector4.h"

#include "system/Platform.h"

#include <iostream>
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

Vector3::Vector3(const Vector4& vec) : x(vec.x), y(vec.y), z(vec.z) {
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
    Vector3 result;
    float length = Length();
    if (length != 0.0f) {
        result.x = x / length;
        result.y = y / length;
        result.z = z / length;
    }
    return result;
}

void Vector3::Normalize()
{
    float length = Length();
    if (length != 0.0f) {
        x /= length;
        y /= length;
        z /= length;
    }
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

Vector3 Vector3::operator*(const Matrix3x3& m) const
{
    // We do the product the same way as if we were multiplying M * v, since
    // it makes more since for linear transformation to do it that way.
    Vector3 v;

    v.x = x * m[0][0] + y * m[0][1] + z * m[0][2];
    v.y = x * m[1][0] + y * m[1][1] + z * m[1][2];
    v.z = x * m[2][0] + y * m[2][1] + z * m[2][2];

    return m * (*this);
}

void Vector3::operator*=(const Matrix3x3& m)
{
    // We do the product the same way as if we were multiplying M * v, since
    // it makes more since for linear transformation to do it that way.
    float nx, ny, nz;

    nx = x * m[0][0] + y * m[0][1] + z * m[0][2];
    ny = x * m[1][0] + y * m[1][1] + z * m[1][2];
    nz = x * m[2][0] + y * m[2][1] + z * m[2][2];

    x = nx;
    y = ny;
    z = nz;
}

Vector3& Vector3::operator=(const Vector4& v) {
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

}
