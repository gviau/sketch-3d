#ifndef SKETCH_3D_MATRIX_4X4_H
#define SKETCH_3D_MATRIX_4X4_H

#include "system/Common.h"

namespace Sketch3D
{

// Forward dependencies
class Vector4;

/**
 * @class Matrix4x4
 * Represents a 4x4 matrix.
 */
class Matrix4x4
{
    public:
        // USEFUL STATICS
        static const Matrix4x4  IDENTITY;   /**< Only the main diagonal is set to 1 */
        static const Matrix4x4  ZERO;       /**< Everything to zero */
        /**
         * Default constructor. Initializes everything to 0.
         */
                                Matrix4x4();

        /**
         * Constructor. Parameters are entered row by row.
         */
                                Matrix4x4(float m11, float m12, float m13, float m14,
                                          float m21, float m22, float m23, float m24,
                                          float m31, float m32, float m33, float m34,
                                          float m41, float m42, float m43, float m44);

        /**
         * Constructor
         * @param data An array of 9 floats to populate the matrix
         */
                                Matrix4x4(float* data);

        /**
         * Copy constructor
         * @param src The matrix to copy
         */
                                Matrix4x4(const Matrix4x4& src);

        /**
         * Return the transpose this matrix
         */
        Matrix4x4               Transpose() const;

        /**
         * Transform this matrix into a rotation matrix around the x axis
         * @param angle The angle in radians to rotate
         */
        void                    RotationAroundX(float angle);

        /**
         * Transform this matrix into a rotation matrix around the y axis
         * @param angle The angle in radians to rotate
         */
        void                    RotationAroundY(float angle);

        /**
         * Transform this matrix into a rotation matrix around the z axis
         * @param angle The angle in radians to rotate
         */
        void                    RotationAroundZ(float angle);

        /**
         * Transform this matrix into a rotation matrix around a vector
         * @param axis The axis to rotate around. The vector is assumed to be
         * normalized.
         * @param angle The angle in radians to rotate
         */
        void                    RotateAroundAxis(const Vector4& axis,
                                                 float angle);

        // ACCESS OPERATORS
        /**
         * Access an element
         * @param col The column to access (x on a 2d grid)
         * @param row The row to access (y on a 2d grid)
         * @return the value contained at (col, row) position
         */
        INLINE float            operator()(int col, int row) const;

        /**
         * Modify an element
         * @param col The column to access (x on a 2d grid)
         * @param row The row to access (y on a 2d grid)
         * @return the value contained at (col, row) position
         */
        INLINE float&            operator()(int col, int row);

        // UNARY OPERATORS
        /**
         * Returns the negated matrix
         */
        INLINE Matrix4x4        operator-() const;

        // BINARY OPERATORS
        INLINE Matrix4x4        operator*(const Matrix4x4& m) const;
        Vector4					operator*(const Vector4& v) const;
        INLINE void             operator*=(const Matrix4x4& m);

        INLINE bool             operator==(const Matrix4x4& m) const;
        INLINE bool             operator!=(const Matrix4x4& m) const;

        INLINE Matrix4x4&       operator=(const Matrix4x4& m);

    private:
        float                   data_[16];   /**< The matrix represented as a linear array */
};

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

#endif
