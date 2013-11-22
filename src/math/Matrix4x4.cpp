#include "Matrix4x4.h"

#include "Constants.h"
#include <math.h>

namespace Sketch3D
{

const Matrix4x4 Matrix4x4::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 0.0f, 1.0f);
const Matrix4x4 Matrix4x4::ZERO;

Matrix4x4::Matrix4x4()
{
    for (int i = 0; i < 16; i++) {
        data_[i] = 0.0f;
    }
}

Matrix4x4::Matrix4x4(float m11, float m12, float m13, float m14,
                     float m21, float m22, float m23, float m24,
                     float m31, float m32, float m33, float m34,
                     float m41, float m42, float m43, float m44)
{
    data_[0] = m11;
    data_[1] = m12;
    data_[2] = m13;
    data_[3] = m14;

    data_[4] = m21;
    data_[5] = m22;
    data_[6] = m23;
    data_[7] = m24;
    
    data_[8] = m31;
    data_[9] = m32;
    data_[10] = m33;
    data_[11] = m34;

    data_[12] = m41;
    data_[13] = m42;
    data_[14] = m43;
    data_[15] = m44;
}

Matrix4x4::Matrix4x4(float* data)
{
    for (int i = 0; i < 16; i++) {
        data_[i] = data[i];
    }
}

Matrix4x4::Matrix4x4(const Matrix4x4& src)
{
    for (int i = 0; i < 16; i++) {
        data_[i] = src.data_[i];
    }
}

Matrix4x4 Matrix4x4::Transpose() const
{
    float data[16];
    int cnt = 0;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            data[cnt++] = data_[j*4 + i];
        }
    }

    return Matrix4x4(data);
}

void Matrix4x4::RotationAroundX(float angle)
{
    float cos = cosf(angle);
    float sin = sinf(angle);

    data_[5] =  cos;
    data_[6] =  sin;
    data_[9] = -sin;
    data_[10] =  cos;

    data_[0] = data_[15] = 1.0f;
    data_[1] = data_[2] = data_[3] = data_[4] = data_[7] = data_[8] =
        data_[11] = data_[12] = data_[13] = data_[14] = 0.0f;
}

void Matrix4x4::RotationAroundY(float angle)
{
    float cos = cosf(angle);
    float sin = sinf(angle);

    data_[0] =  cos;
    data_[2] = -sin;
    data_[8] =  sin;
    data_[10] =  cos;

    data_[5] = data_[15] = 1.0f;
    data_[1] = data_[3] = data_[4] = data_[6] = data_[7] = data_[9] = data_[11] =
        data_[12] = data_[13] = data_[14] = 0.0f;
}

void Matrix4x4::RotationAroundZ(float angle)
{
    float cos = cosf(angle);
    float sin = sinf(angle);

    data_[0] =  cos;
    data_[1] =  sin;
    data_[4] = -sin;
    data_[5] =  cos;

    data_[10] = data_[15] = 1.0f;
    data_[2] = data_[3] = data_[6] = data_[7] = data_[8] = data_[9] = data_[11] =
        data_[12] = data_[13] = data_[14] = 0.0f;
}

void Matrix4x4::RotateAroundAxis(const Vector4& axis, float angle)
{
    float cos = cosf(angle);
    float sin = sinf(angle);
    float sum = 1.0f - cos;

    data_[0] = axis.x * axis.x * sum + cos;
    data_[1] = axis.x * axis.y * sum - (axis.z * sin);
    data_[2] = axis.x * axis.z * sum + (axis.y * sin);

    data_[4] = axis.y * axis.x * sum + (axis.z * sin);
    data_[5] = axis.y * axis.y * sum + cos;
    data_[6] = axis.y * axis.z * sum - (axis.x * sin);

    data_[8] = axis.z * axis.x * sum - (axis.y * sin);
    data_[9] = axis.z * axis.y * sum + (axis.x * sin);
    data_[10] = axis.z * axis.z * sum + cos;

    data_[3] = data_[7] = data_[11] = data_[12] = data_[13] = data_[14] = 0.0f;
    data_[15] = 1.0f;
}

INLINE float Matrix4x4::operator()(int col, int row) const
{
    return data_[row*4 + col];
}

INLINE float& Matrix4x4::operator()(int col, int row)
{
    return data_[row*4 + col];
}

INLINE Matrix4x4 Matrix4x4::operator-() const
{
    float data[16];
    for (int i = 0; i < 16; i++) {
        data[i] = -data_[i];
    }

    return Matrix4x4(data);
}

INLINE Matrix4x4 Matrix4x4::operator*(const Matrix4x4& m) const
{
    Matrix4x4 mat;

    for (int i = 0; i < 16; i += 4) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                mat.data_[i + j] += data_[i + k] * m.data_[k*4 + j];
            }
        }
    }

    return mat;
}

INLINE Vector4 Matrix4x4::operator*(const Vector4& v) const
{
    Vector4 w;

    w.x = v.x * data_[0]  + v.y * data_[1]  + v.z * data_[2]  + data_[3];
    w.y = v.x * data_[4]  + v.y * data_[5]  + v.z * data_[6]  + data_[7];
    w.z = v.x * data_[8]  + v.y * data_[9]  + v.z * data_[10] + data_[11];
    w.w = v.x * data_[12] + v.y * data_[13] + v.z * data_[14] + data_[15];

    w.x /= w.w;
    w.y /= w.w;
    w.z /= w.w;
    w.w = 1.0f;

    return w;
}

INLINE void Matrix4x4::operator*=(const Matrix4x4& m)
{
    Matrix4x4 mat;

    for (int i = 0; i < 16; i += 4) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                mat.data_[i + j] += data_[i + k] * m.data_[k*4 + j];
            }
        }
    }

    (*this) = mat;
}

INLINE bool Matrix4x4::operator==(const Matrix4x4& m) const
{
    for (int i = 0; i < 16; i++) {
        if (fabs(data_[i] - m.data_[i]) > EPSILON) {
            return false;
        }
    }

    return true;
}

INLINE bool Matrix4x4::operator!=(const Matrix4x4& m) const
{
    for (int i = 0; i < 16; i++) {
        if (fabs(data_[i] - m.data_[i]) > EPSILON) {
            return true;
        }
    }

    return false;
}

INLINE Matrix4x4& Matrix4x4::operator=(const Matrix4x4& m)
{
    if (this == &m) {
        return *this;
    }

    for (int i = 0; i < 16; i++) {
        data_[i] = m.data_[i];
    }

    return *this;
}

}

