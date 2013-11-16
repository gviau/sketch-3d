#include "Quaternion.h"

#include <math.h>

#include "Constants.h"
#include "Vector3.h"

namespace Sketch3D
{
Quaternion::Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f)
{
}

Quaternion::Quaternion(float w, float x, float y, float z)
{
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}

float Quaternion::Magnitude() const
{
    return sqrtf(w*w + x*x + y*y + z*z);
}

Quaternion Quaternion::Normalized() const
{
    float f = Magnitude();
    return Quaternion(w / f, x / f, y / f, z / f);
}

void Quaternion::Normalize()
{
    float f = Magnitude();
    w /= f;
    x /= f;
    y /= f;
    z /= f;
}

void Quaternion::ConstructRotationMatrix(Matrix4x4& mat) const
{
    float xx, xy, xz, xw, yy, yz, yw, zz, zw;

    xx = x * x;
    xy = x * y;
    xz = x * z;
    xw = x * w;

    yy = y * y;
    yz = y * z;
    yw = y * w;

    zz = z * z;
    zw = z * w;

    mat(0, 0) = 1.0f - 2.0f * (yy + zz);
    mat(0, 1) = 2.0f * (xy - zw);
    mat(0, 2) = 2.0f * (xz + yw);

    mat(1, 0) = 2.0f * (xy + zw);
    mat(1, 1) = 1.0f - 2.0f * (xx + zz);
    mat(1, 2) = 2.0f * (yz - xw);

    mat(2, 0) = 2.0f * (xz - yw);
    mat(2, 1) = 2.0f * (yz + xw);
    mat(2, 2) = 1.0f - 2.0f * (xx + yy);

    mat(3, 3) = 1.0f;
}

void Quaternion::MakeFromAngleAxis(float angle, const Vector3& axis)
{
    angle *= 0.5f;

    float s = sinf(angle);

    x = axis.x * s;
    y = axis.y * s;
    z = axis.z * s;
    w = cosf(angle);
}

Quaternion Quaternion::operator*(const Quaternion& q) const
{
    return Quaternion(w*q.w - x*q.x - y*q.y - z*q.z,
                      w*q.x + x*q.w + y*q.z - z*q.y,
                      w*q.y + y*q.w + z*q.x - x*q.z,
                      w*q.z + z*q.w + x*q.y - y*q.x);
}

void Quaternion::operator*=(const Quaternion& q)
{
    float w_, x_, y_, z_;

    w_ = w*q.w - x*q.x - y*q.y - z*q.z;
    x_ = w*q.x + x*q.w + y*q.z - z*q.y;
    y_ = w*q.y + y*q.w + z*q.x - x*q.z;
    z_ = w*q.z + z*q.w + x*q.y - y*q.x;

    w = w_;
    x = x_;
    y = y_;
    z = z_;
}

bool Quaternion::operator==(const Quaternion& q) const
{
    return fabs(w - q.w) <= EPSILON &&
           fabs(x - q.x) <= EPSILON &&
           fabs(y - q.y) <= EPSILON &&
           fabs(z - q.z) <= EPSILON;
}

bool Quaternion::operator!=(const Quaternion& q) const
{
    return fabs(w - q.w) > EPSILON ||
           fabs(x - q.x) > EPSILON ||
           fabs(y - q.y) > EPSILON ||
           fabs(z - q.z) > EPSILON;
}
}
