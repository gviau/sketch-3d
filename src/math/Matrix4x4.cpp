#include "math/Matrix4x4.h"

#include "math/Constants.h"
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

void Matrix4x4::RotateAroundAxis(const Vector4& axis, float angle)
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

Vector4 Matrix4x4::operator*(const Vector4& v) const
{
    Vector4 w;

    w.x = v.x * data_[0][0] + v.y * data_[0][1] + v.z * data_[0][2] + data_[0][3];
    w.y = v.x * data_[1][0] + v.y * data_[1][1] + v.z * data_[1][2] + data_[1][3];
    w.z = v.x * data_[2][0] + v.y * data_[2][1] + v.z * data_[2][2] + data_[2][3];
    w.w = v.x * data_[3][0] + v.y * data_[3][1] + v.z * data_[3][2] + data_[3][3];

    w.x /= w.w;
    w.y /= w.w;
    w.z /= w.w;
    w.w = 1.0f;

    return w;
}

}

