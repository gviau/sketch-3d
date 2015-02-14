#include "math/Quaternion.h"

#include "math/Constants.h"
#include "math/Vector3.h"

#include <math.h>

namespace Sketch3D
{

const Quaternion Quaternion::IDENTITY;
const Quaternion Quaternion::ZERO(0.0f, 0.0f, 0.0f, 0.0f);

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

float Quaternion::Dot(const Quaternion& q) const {
    return w*q.w + x*q.x + y*q.y + z*q.z;
}

void Quaternion::MakeFromRotationMatrix(const Matrix3x3& mat)
{
    Matrix4x4 m;
    m[0][0] = mat[0][0];
    m[0][1] = mat[0][1];
    m[0][2] = mat[0][2];
				
    m[1][0] = mat[1][0];
    m[1][1] = mat[1][1];
    m[1][2] = mat[1][2];
				
    m[2][0] = mat[2][0];
    m[2][1] = mat[2][1];
    m[2][2] = mat[2][2];

    MakeFromRotationMatrix(m);
}

void Quaternion::MakeFromRotationMatrix(const Matrix4x4& mat)
{
    // Taken from Ogre3D
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

    float trace = mat[0][0] + mat[1][1] + mat[2][2];
    float root;

    if (trace > 0.0f) {
        root = sqrtf(trace + 1.0f);
        w = 0.5f * root;
        root = 0.5f / root;
        x = (mat[2][1] - mat[1][2]) * root;
        y = (mat[0][2] - mat[2][0]) * root;
        z = (mat[1][0] - mat[0][1]) * root;
    } else {
        int i = 0;
        if (mat[1][1] > mat[0][0]) {
            i = 1;
        }
        if (mat[2][2] > mat[i][i]) {
            i = 2;
        }

        int j = (i + 1) % 3;
        int k = (j + 1) % 3;

        root = sqrtf(mat[i][i] - mat[j][j] - mat[k][k] + 1.0f);
        float* apkQuat[3] = { &x, &y, &z };
        *apkQuat[i] = root * 0.5f;
        root = 0.5f / root;
        w = (mat[k][j] - mat[j][k]) * root;
        *apkQuat[j] = (mat[j][i] + mat[i][j]) * root;
        *apkQuat[k] = (mat[k][i] + mat[i][k]) * root;
    }
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

void Quaternion::MakeFromAxes(const Vector3& xAxis, const Vector3& yAxis,
                              const Vector3& zAxis)
{
    Matrix3x3 mat;

    mat[0][0] = xAxis.x;
    mat[1][0] = xAxis.y;
    mat[2][0] = xAxis.z;
	   		
    mat[0][1] = yAxis.x;
    mat[1][1] = yAxis.y;
    mat[2][1] = yAxis.z;
	   		
    mat[0][2] = zAxis.x;
    mat[1][2] = zAxis.y;
    mat[2][2] = zAxis.z;

    MakeFromRotationMatrix(mat);
}

void Quaternion::ToRotationMatrix(Matrix3x3& mat) const
{
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = x + x;
	y2 = y + y;
	z2 = z + z;

	xx = x * x2;
	xy = x * y2;
	xz = x * z2;

	yy = y * y2;
	yz = y * z2;
	zz = z * z2;

	wx = w * x2;
	wy = w * y2;
	wz = w * z2;

	mat[0][0] = 1.0f - (yy + zz);
	mat[0][1] = xy - wz;
	mat[0][2] = xz + wy;

	mat[1][0] = xy + wz;
	mat[1][1] = 1.0f - (xx + zz);
	mat[1][2] = yz - wx;

	mat[2][0] = xz - wy;
	mat[2][1] = yz + wx;
	mat[2][2] = 1.0f - (xx + yy);
	/*
    float xx, xy, xz, xw, yy, yz, yw, zz, zw, tx, ty, tz;

    tx = x + x;
    ty = y + y;
    tz = z + z;

    xx = tx * x;
    xy = tx * y;
    xz = tx * z;
    xw = tx * w;

    yy = ty * y;
    yz = ty * z;
    yw = ty * w;

    zz = tz * z;
    zw = tz * w;

    mat[0][0] = 1.0f - (yy + zz);
    mat[0][1] = (xy - zw);
    mat[0][2] = (xz + yw);
		 
    mat[1][0] = (xy + zw);
    mat[1][1] = 1.0f - (xx + zz);
    mat[1][2] = (yz - xw);
		 
    mat[2][0] = (xz - yw);
    mat[2][1] = (yz + xw);
    mat[2][2] = 1.0f - (xx + yy);
	*/
}

void Quaternion::ToRotationMatrix(Matrix4x4& mat) const
{
    Matrix3x3 m;
    ToRotationMatrix(m);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mat[i][j] = m[i][j];
        }
    }

    mat[3][3] = 1.0f;
}

void Quaternion::ToAngleAxis(float& angle, Vector3& axis) const
{
    float sqrLength = x*x + y*y + z*z;
    if (sqrLength > 0.0f) {
        angle = 2.0f * acosf(w);
        float invLength = 1.0f / sqrtf(sqrLength);
        axis.x = x * invLength;
        axis.y = y * invLength;
        axis.z = z * invLength;
    } else {
        angle = 0.0f;
        axis.x = 1.0f;
        axis.y = 0.0f;
        axis.z = 0.0f;
    }
}

void Quaternion::ToAxes(Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const
{
    Matrix3x3 mat;
    ToRotationMatrix(mat);

    xAxis.x = mat[0][0];
    xAxis.y = mat[1][0];
    xAxis.z = mat[2][0];
				 		
    yAxis.x = mat[0][1];
    yAxis.y = mat[1][1];
    yAxis.z = mat[2][1];
				 		
    zAxis.x = mat[0][2];
    zAxis.y = mat[1][2];
    zAxis.z = mat[2][2];
}

Quaternion Quaternion::Slerp(const Quaternion& endQuat, float t, bool shortestPath) const {
    float cos_ = Dot(endQuat);
    Quaternion end = endQuat;

    if (cos_ < 0.0f && shortestPath) {
        cos_ = -cos_;
        end = -endQuat;
    }

    if (abs(cos_) < 1.0f - EPSILON) {
        float sin_ = sqrtf(1.0f - cos_ * cos_);
        float angle = atan2(sin_, cos_);
        float invSin = 1.0f / sin_;
        float coeff0 = sin((1.0f - t) * angle) * invSin;
        float coeff1 = sin(t * angle) * invSin;
        
        return (*this) * coeff0 + end * coeff1;
    }

    Quaternion interpolated = (*this) * (1.0f - t) + end * t;
    interpolated.Normalize();
    return interpolated;
}

Vector3 Quaternion::GetXAxis() const
{
    return Vector3(1.0f - 2.0f * (y*y + z*z),
                   2.0f * (y*x + z*w),
                   2.0f * (x*z - y*w));
}

Vector3 Quaternion::GetYAxis() const
{
    return Vector3(2.0f * (x*y - w*z),
                   1.0f - 2.0f * (x*x + z*z),
                   2.0f * (y*z + w*x));
}

Vector3 Quaternion::GetZAxis() const
{
    return Vector3(2.0f * (x*z + w*y),
                   2.0f * (y*z - w*x),
                   1.0f - 2.0f * (x*x + y*y));
}

Quaternion Quaternion::operator*(const Quaternion& q) const
{
    return Quaternion(w*q.w - x*q.x - y*q.y - z*q.z,
                      w*q.x + x*q.w + y*q.z - z*q.y,
                      w*q.y + y*q.w + z*q.x - x*q.z,
                      w*q.z + z*q.w + x*q.y - y*q.x);
}

Quaternion Quaternion::operator-() const {
    return Quaternion(-w, -x, -y, -z);
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
