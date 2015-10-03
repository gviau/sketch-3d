#include "math/Matrix4x4.h"

#include "math/Constants.h"
#include "math/Matrix3x3.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include <math.h>

namespace Sketch3D
{

const Matrix4x4 Matrix4x4::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 0.0f, 1.0f);
const Matrix4x4 Matrix4x4::ZERO(0.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 0.0f);

Matrix4x4::Matrix4x4()
{
    for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			data_[i][j] = 0.0f;
		}
    }
	data_[0][0] = data_[1][1] = data_[2][2] = data_[3][3] = 1.0f;
}

Matrix4x4::Matrix4x4(float m11, float m12, float m13, float m14,
                     float m21, float m22, float m23, float m24,
                     float m31, float m32, float m33, float m34,
                     float m41, float m42, float m43, float m44)
{
    data_[0][0] = m11;
    data_[0][1] = m12;
    data_[0][2] = m13;
    data_[0][3] = m14;

    data_[1][0] = m21;
    data_[1][1] = m22;
    data_[1][2] = m23;
    data_[1][3] = m24;
    
    data_[2][0] = m31;
    data_[2][1] = m32;
    data_[2][2] = m33;
    data_[2][3] = m34;

    data_[3][0] = m41;
    data_[3][1] = m42;
    data_[3][2] = m43;
    data_[3][3] = m44;
}

Matrix4x4::Matrix4x4(float* data)
{
	int idx = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			data_[i][j] = data[idx++];
		}
	}
}

Matrix4x4::Matrix4x4(const Matrix4x4& src)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			data_[i][j] = src.data_[i][j];
		}
	}
}

Matrix4x4::Matrix4x4(const Matrix3x3& src) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			data_[i][j] = src[i][j];
		}
	}
}

Matrix4x4 Matrix4x4::Transpose() const
{
    float data[16];
    int cnt = 0;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            data[cnt++] = data_[j][i];
        }
    }

    return Matrix4x4(data);
}

Matrix4x4 Matrix4x4::Inverse() const {
    // From OGRE source code
    float m00 = data_[0][0], m01 = data_[0][1], m02 = data_[0][2], m03 = data_[0][3];
    float m10 = data_[1][0], m11 = data_[1][1], m12 = data_[1][2], m13 = data_[1][3];
    float m20 = data_[2][0], m21 = data_[2][1], m22 = data_[2][2], m23 = data_[2][3];
    float m30 = data_[3][0], m31 = data_[3][1], m32 = data_[3][2], m33 = data_[3][3];

    float v0 = m20 * m31 - m21 * m30;
    float v1 = m20 * m32 - m22 * m30;
    float v2 = m20 * m33 - m23 * m30;
    float v3 = m21 * m32 - m22 * m31;
    float v4 = m21 * m33 - m23 * m31;
    float v5 = m22 * m33 - m23 * m32;

    float t00 =  (v5 * m11 - v4 * m12 + v3 * m13);
    float t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
    float t20 =  (v4 * m10 - v2 * m11 + v0 * m13);
    float t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

    float invDet = 1.0f / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

    float d00 = t00 * invDet;
    float d10 = t10 * invDet;
    float d20 = t20 * invDet;
    float d30 = t30 * invDet;

    float d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d11 =  (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d31 =  (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    v0 = m10 * m31 - m11 * m30;
    v1 = m10 * m32 - m12 * m30;
    v2 = m10 * m33 - m13 * m30;
    v3 = m11 * m32 - m12 * m31;
    v4 = m11 * m33 - m13 * m31;
    v5 = m12 * m33 - m13 * m32;

    float d02 =  (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d22 =  (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    v0 = m21 * m10 - m20 * m11;
    v1 = m22 * m10 - m20 * m12;
    v2 = m23 * m10 - m20 * m13;
    v3 = m22 * m11 - m21 * m12;
    v4 = m23 * m11 - m21 * m13;
    v5 = m23 * m12 - m22 * m13;

    float d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d13 =  (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d33 =  (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    return Matrix4x4(
        d00, d01, d02, d03,
        d10, d11, d12, d13,
        d20, d21, d22, d23,
        d30, d31, d32, d33);
}

void Matrix4x4::Translate(const Vector3& translation) {
    data_[0][3] += translation.x;
    data_[1][3] += translation.y;
    data_[2][3] += translation.z;
}

void Matrix4x4::RotationAroundX(float angle)
{
    float cos = cosf(angle);
    float sin = sinf(angle);

    data_[1][1] =  cos;
    data_[1][2] =  sin;
    data_[2][1] = -sin;
    data_[2][2] =  cos;

    data_[0][0] = data_[3][3] = 1.0f;
    data_[0][1] = data_[0][2] = data_[0][3] = data_[1][0] = data_[1][3] = data_[2][0] =
        data_[2][3] = data_[3][0] = data_[3][1] = data_[3][2] = 0.0f;
}

void Matrix4x4::RotationAroundY(float angle)
{
    float cos = cosf(angle);
    float sin = sinf(angle);

    data_[0][0] =  cos;
    data_[0][2] = -sin;
    data_[2][0] =  sin;
    data_[2][2] =  cos;

    data_[1][1] = data_[3][3] = 1.0f;
    data_[0][1] = data_[0][3] = data_[1][0] = data_[1][2] = data_[1][3] = data_[2][1] = data_[2][3] =
        data_[3][0] = data_[3][1] = data_[3][2] = 0.0f;
}

void Matrix4x4::RotationAroundZ(float angle)
{
    float cos = cosf(angle);
    float sin = sinf(angle);

    data_[0][0] =  cos;
    data_[0][1] =  sin;
    data_[1][0] = -sin;
    data_[1][1] =  cos;

    data_[2][2] = data_[3][3] = 1.0f;
    data_[0][2] = data_[0][3] = data_[1][2] = data_[1][3] = data_[2][0] = data_[2][1] = data_[2][3] =
        data_[3][0] = data_[3][1] = data_[3][2] = 0.0f;
}

void Matrix4x4::RotateAroundAxis(const Vector3& axis, float angle)
{
    float cos = cosf(angle);
    float sin = sinf(angle);
    float sum = 1.0f - cos;

    data_[0][0] = axis.x * axis.x * sum + cos;
    data_[0][1] = axis.x * axis.y * sum - (axis.z * sin);
    data_[0][2] = axis.x * axis.z * sum + (axis.y * sin);

    data_[1][0] = axis.y * axis.x * sum + (axis.z * sin);
    data_[1][1] = axis.y * axis.y * sum + cos;
    data_[1][2] = axis.y * axis.z * sum - (axis.x * sin);

    data_[2][0] = axis.z * axis.x * sum - (axis.y * sin);
    data_[2][1] = axis.z * axis.y * sum + (axis.x * sin);
    data_[2][2] = axis.z * axis.z * sum + cos;

    data_[0][3] = data_[1][3] = data_[2][3] = data_[3][0] = data_[3][1] = data_[3][2] = 0.0f;
    data_[3][3] = 1.0f;
}

void Matrix4x4::Scale(const Vector3& scale) {
    *this = Matrix4x4::IDENTITY;
    data_[0][0] = scale.x;
    data_[1][1] = scale.y;
    data_[2][2] = scale.z;
}

Vector3 Matrix4x4::GetXAxis() const
{
    return Vector3(data_[0][0], data_[1][0], data_[2][0]);
}

Vector3 Matrix4x4::GetYAxis() const
{
    return Vector3(data_[0][1], data_[1][1], data_[2][1]);
}

Vector3 Matrix4x4::GetZAxis() const
{
    return Vector3(data_[0][2], data_[1][2], data_[2][2]);
}

Vector3 Matrix4x4::GetWAxis() const
{
    return Vector3(data_[0][3], data_[1][3], data_[2][3]);
}

Vector4 Matrix4x4::operator*(const Vector3& v) const {
    Vector4 w(v.x, v.y, v.z);
    return *this * w;
}

Vector4 Matrix4x4::operator*(const Vector4& v) const
{
    Vector4 w;

    w.x = v.x * data_[0][0] + v.y * data_[0][1] + v.z * data_[0][2] + v.w * data_[0][3];
    w.y = v.x * data_[1][0] + v.y * data_[1][1] + v.z * data_[1][2] + v.w * data_[1][3];
    w.z = v.x * data_[2][0] + v.y * data_[2][1] + v.z * data_[2][2] + v.w * data_[2][3];
    w.w = v.x * data_[3][0] + v.y * data_[3][1] + v.z * data_[3][2] + v.w * data_[3][3];

    return w;
}

Matrix4x4& Matrix4x4::operator=(const Matrix3x3& m)
{
    for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			data_[i][j] = m[i][j];
		}
    }

    return *this;
}

}

