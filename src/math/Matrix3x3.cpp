#include "math/Matrix3x3.h"

#include "math/Constants.h"
#include "math/Matrix4x4.h"
#include "math/Vector2.h"
#include "math/Vector3.h"

#include <math.h>

namespace Sketch3D
{

const Matrix3x3 Matrix3x3::IDENTITY(1.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f);
const Matrix3x3 Matrix3x3::ZERO(0.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.0f);

Matrix3x3::Matrix3x3()
{
    for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			data_[i][j] = 0.0f;
		}
    }
	data_[0][0] = data_[1][1] = data_[2][2] = 1.0f;
}

Matrix3x3::Matrix3x3(float m11, float m12, float m13,
                     float m21, float m22, float m23,
                     float m31, float m32, float m33)
{
    data_[0][0] = m11;
    data_[0][1] = m12;
    data_[0][2] = m13;

    data_[1][0] = m21;
    data_[1][1] = m22;
    data_[1][2] = m23;
    
    data_[2][0] = m31;
    data_[2][1] = m32;
    data_[2][2] = m33;
}

Matrix3x3::Matrix3x3(float* data)
{
	int idx = 0;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			data_[i][j] = data[idx++];
		}
	}
}

Matrix3x3::Matrix3x3(const Matrix3x3& src)
{
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			data_[i][j] = src.data_[i][j];
		}
	}
}

Matrix3x3::Matrix3x3(const Matrix4x4& src)
{
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			data_[i][j] = src[i][j];
		}
	}
}

Matrix3x3 Matrix3x3::Transpose() const
{
    float data[9];
    int cnt = 0;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            data[cnt++] = data_[j][i];
        }
    }

    return Matrix3x3(data);
}

void Matrix3x3::Translate(const Vector2& translation) {
    *this = Matrix3x3::IDENTITY;
    data_[0][2] = translation.x;
    data_[1][2] = translation.y;
}

void Matrix3x3::RotationAroundX(float angle)
{
    float cos = cosf(angle);
    float sin = sinf(angle);

    data_[1][1] =  cos;
    data_[1][2] =  sin;
    data_[2][1] = -sin;
    data_[2][2] =  cos;

    data_[0][0] = 1.0f;
    data_[0][1] = data_[0][2] = data_[1][0] = data_[2][0] = 0.0f;
}

void Matrix3x3::RotationAroundY(float angle)
{
    float cos = cosf(angle);
    float sin = sinf(angle);

    data_[0][0] =  cos;
    data_[0][2] = -sin;
    data_[2][0] =  sin;
    data_[2][2] =  cos;

    data_[1][1] = 1.0f;
    data_[0][1] = data_[1][0] = data_[1][2] = data_[2][1] = 0.0f;
}

void Matrix3x3::RotationAroundZ(float angle)
{
    float cos = cosf(angle);
    float sin = sinf(angle);

    data_[0][0] =  cos;
    data_[0][1] =  sin;
    data_[1][0] = -sin;
    data_[1][1] =  cos;

    data_[2][2] = 1.0f;
    data_[0][2] = data_[1][2] = data_[2][0] = data_[2][1] = 0.0f;
}

void Matrix3x3::RotateAroundAxis(const Vector3& axis, float angle)
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
}

void Matrix3x3::Scale(const Vector3& scale) {
    *this = Matrix4x4::IDENTITY;
    data_[0][0] = scale.x;
    data_[1][1] = scale.y;
    data_[2][2] = scale.z;
}

Vector3 Matrix3x3::operator*(const Vector2& v) const {
    Vector3 w(v.x, v.y, 1.0f);
    return *this * w;
}

Vector3 Matrix3x3::operator*(const Vector3& v) const
{
    Vector3 w;

    w.x = v.x * data_[0][0] + v.y * data_[0][1] + v.z * data_[0][2];
    w.y = v.x * data_[1][0] + v.y * data_[1][1] + v.z * data_[1][2];
    w.z = v.x * data_[2][0] + v.y * data_[2][1] + v.z * data_[2][2];

    return w;
}

Matrix3x3& Matrix3x3::operator=(const Matrix4x4& m)
{
    for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			data_[i][j] = m[i][j];
		}
    }

    return *this;
}

}
