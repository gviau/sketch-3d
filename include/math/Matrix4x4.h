#ifndef SKETCH_3D_MATRIX_4X4_H
#define SKETCH_3D_MATRIX_4X4_H

#include "system/Common.h"

namespace Sketch3D
{

// Forward dependencies
class Matrix3x3;
class Vector3;
class Vector4;

/**
 * @class Matrix4x4
 * Represents a column-major 4x4 matrix.
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
         * @param data An array of 16 floats to populate the matrix
         */
                                Matrix4x4(float* data);

        /**
         * Copy constructor
         * @param src The matrix to copy
         */
                                Matrix4x4(const Matrix4x4& src);

		/**
		 * Copy constructor. Assign only the 3x3 sub matrix part
		 * @param src The matrix to copy
		 */
								Matrix4x4(const Matrix3x3& src);

        /**
         * Return the transpose this matrix
         */
        Matrix4x4               Transpose() const;

        /**
         * Transform this matrix into a translation matrix
         * @param translation A vector representing the translation
         */
        void                    Translate(const Vector3& translation);

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
        void                    RotateAroundAxis(const Vector3& axis,
                                                 float angle);

        /**
         * Transform this matrix into a scale matrix
         * @param scale A vector representing the amount to scale on each axis
         */
        void                    Scale(const Vector3& scale);

        // ACCESS OPERATORS
        /**
         * Access a row of the matrix
         * @param row The row to access
         * @return The row contained at 'row' index
         */
        INLINE const float*		operator[] (int row) const;

        /**
         * Modify a row of the matrix
         * @param row The row to access
         * @return The row contained at 'row' index
         */
		INLINE float*            operator[] (int row);

        // UNARY OPERATORS
        /**
         * Returns the negated matrix
         */
        INLINE Matrix4x4        operator-() const;

        // BINARY OPERATORS
        INLINE Matrix4x4        operator*(const Matrix4x4& m) const;
        Vector4                 operator*(const Vector3& v) const;
        Vector4					operator*(const Vector4& v) const;
        INLINE void             operator*=(const Matrix4x4& m);

        INLINE bool             operator==(const Matrix4x4& m) const;
        INLINE bool             operator!=(const Matrix4x4& m) const;

		Matrix4x4&				operator=(const Matrix3x3& m);
        INLINE Matrix4x4&       operator=(const Matrix4x4& m);

		/**
		 * Return the matrix as a column-major linear array.
		 * @param The returned array
		 */
		INLINE void				GetData(float* data) const;

    private:
		float                   data_[4][4];	/**< The matrix represented as 2d array */
};

INLINE const float* Matrix4x4::operator[] (int row) const
{
    return data_[row];
}

INLINE float* Matrix4x4::operator[] (int row) {
    return data_[row];
}

INLINE Matrix4x4 Matrix4x4::operator-() const
{
    float data[16];
	int idx = 0;
    for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			data[idx] = -data_[i][j];
			idx += 1;
		}
    }

    return Matrix4x4(data);
}

INLINE Matrix4x4 Matrix4x4::operator*(const Matrix4x4& m) const
{
    Matrix4x4 mat;
    mat.data_[0][0] = data_[0][0] * m.data_[0][0] + data_[0][1] * m.data_[1][0] + data_[0][2] * m.data_[2][0] + data_[0][3] * m.data_[3][0];
    mat.data_[0][1] = data_[0][0] * m.data_[0][1] + data_[0][1] * m.data_[1][1] + data_[0][2] * m.data_[2][1] + data_[0][3] * m.data_[3][1];
    mat.data_[0][2] = data_[0][0] * m.data_[0][2] + data_[0][1] * m.data_[1][2] + data_[0][2] * m.data_[2][2] + data_[0][3] * m.data_[3][2];
    mat.data_[0][3] = data_[0][0] * m.data_[0][3] + data_[0][1] * m.data_[1][3] + data_[0][2] * m.data_[2][3] + data_[0][3] * m.data_[3][3];

    mat.data_[1][0] = data_[1][0] * m.data_[0][0] + data_[1][1] * m.data_[1][0] + data_[1][2] * m.data_[2][0] + data_[1][3] * m.data_[3][0];
    mat.data_[1][1] = data_[1][0] * m.data_[0][1] + data_[1][1] * m.data_[1][1] + data_[1][2] * m.data_[2][1] + data_[1][3] * m.data_[3][1];
    mat.data_[1][2] = data_[1][0] * m.data_[0][2] + data_[1][1] * m.data_[1][2] + data_[1][2] * m.data_[2][2] + data_[1][3] * m.data_[3][2];
    mat.data_[1][3] = data_[1][0] * m.data_[0][3] + data_[1][1] * m.data_[1][3] + data_[1][2] * m.data_[2][3] + data_[1][3] * m.data_[3][3];

    mat.data_[2][0] = data_[2][0] * m.data_[0][0] + data_[2][1] * m.data_[1][0] + data_[2][2] * m.data_[2][0] + data_[2][3] * m.data_[3][0];
    mat.data_[2][1] = data_[2][0] * m.data_[0][1] + data_[2][1] * m.data_[1][1] + data_[2][2] * m.data_[2][1] + data_[2][3] * m.data_[3][1];
    mat.data_[2][2] = data_[2][0] * m.data_[0][2] + data_[2][1] * m.data_[1][2] + data_[2][2] * m.data_[2][2] + data_[2][3] * m.data_[3][2];
    mat.data_[2][3] = data_[2][0] * m.data_[0][3] + data_[2][1] * m.data_[1][3] + data_[2][2] * m.data_[2][3] + data_[2][3] * m.data_[3][3];

    mat.data_[3][0] = data_[3][0] * m.data_[0][0] + data_[3][1] * m.data_[1][0] + data_[3][2] * m.data_[2][0] + data_[3][3] * m.data_[3][0];
    mat.data_[3][1] = data_[3][0] * m.data_[0][1] + data_[3][1] * m.data_[1][1] + data_[3][2] * m.data_[2][1] + data_[3][3] * m.data_[3][1];
    mat.data_[3][2] = data_[3][0] * m.data_[0][2] + data_[3][1] * m.data_[1][2] + data_[3][2] * m.data_[2][2] + data_[3][3] * m.data_[3][2];
    mat.data_[3][3] = data_[3][0] * m.data_[0][3] + data_[3][1] * m.data_[1][3] + data_[3][2] * m.data_[2][3] + data_[3][3] * m.data_[3][3];

    return mat;
}

INLINE void Matrix4x4::operator*=(const Matrix4x4& m)
{
	Matrix4x4 mat = ZERO;
    mat.data_[0][0] = data_[0][0] * m.data_[0][0] + data_[0][1] * m.data_[1][0] + data_[0][2] * m.data_[2][0] + data_[0][3] * m.data_[3][0];
    mat.data_[0][1] = data_[0][0] * m.data_[0][1] + data_[0][1] * m.data_[1][1] + data_[0][2] * m.data_[2][1] + data_[0][3] * m.data_[3][1];
    mat.data_[0][2] = data_[0][0] * m.data_[0][2] + data_[0][1] * m.data_[1][2] + data_[0][2] * m.data_[2][2] + data_[0][3] * m.data_[3][2];
    mat.data_[0][3] = data_[0][0] * m.data_[0][3] + data_[0][1] * m.data_[1][3] + data_[0][2] * m.data_[2][3] + data_[0][3] * m.data_[3][3];

    mat.data_[1][0] = data_[1][0] * m.data_[0][0] + data_[1][1] * m.data_[1][0] + data_[1][2] * m.data_[2][0] + data_[1][3] * m.data_[3][0];
    mat.data_[1][1] = data_[1][0] * m.data_[0][1] + data_[1][1] * m.data_[1][1] + data_[1][2] * m.data_[2][1] + data_[1][3] * m.data_[3][1];
    mat.data_[1][2] = data_[1][0] * m.data_[0][2] + data_[1][1] * m.data_[1][2] + data_[1][2] * m.data_[2][2] + data_[1][3] * m.data_[3][2];
    mat.data_[1][3] = data_[1][0] * m.data_[0][3] + data_[1][1] * m.data_[1][3] + data_[1][2] * m.data_[2][3] + data_[1][3] * m.data_[3][3];

    mat.data_[2][0] = data_[2][0] * m.data_[0][0] + data_[2][1] * m.data_[1][0] + data_[2][2] * m.data_[2][0] + data_[2][3] * m.data_[3][0];
    mat.data_[2][1] = data_[2][0] * m.data_[0][1] + data_[2][1] * m.data_[1][1] + data_[2][2] * m.data_[2][1] + data_[2][3] * m.data_[3][1];
    mat.data_[2][2] = data_[2][0] * m.data_[0][2] + data_[2][1] * m.data_[1][2] + data_[2][2] * m.data_[2][2] + data_[2][3] * m.data_[3][2];
    mat.data_[2][3] = data_[2][0] * m.data_[0][3] + data_[2][1] * m.data_[1][3] + data_[2][2] * m.data_[2][3] + data_[2][3] * m.data_[3][3];

    mat.data_[3][0] = data_[3][0] * m.data_[0][0] + data_[3][1] * m.data_[1][0] + data_[3][2] * m.data_[2][0] + data_[3][3] * m.data_[3][0];
    mat.data_[3][1] = data_[3][0] * m.data_[0][1] + data_[3][1] * m.data_[1][1] + data_[3][2] * m.data_[2][1] + data_[3][3] * m.data_[3][1];
    mat.data_[3][2] = data_[3][0] * m.data_[0][2] + data_[3][1] * m.data_[1][2] + data_[3][2] * m.data_[2][2] + data_[3][3] * m.data_[3][2];
    mat.data_[3][3] = data_[3][0] * m.data_[0][3] + data_[3][1] * m.data_[1][3] + data_[3][2] * m.data_[2][3] + data_[3][3] * m.data_[3][3];

    (*this) = mat;
}

INLINE bool Matrix4x4::operator==(const Matrix4x4& m) const
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (fabs(data_[i][j] - m.data_[i][j]) > EPSILON) {
				return false;
			}
		}
	}

    return true;
}

INLINE bool Matrix4x4::operator!=(const Matrix4x4& m) const
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (fabs(data_[i][j] - m.data_[i][j]) > EPSILON) {
				return true;
			}
		}
	}

    return false;
}

INLINE Matrix4x4& Matrix4x4::operator=(const Matrix4x4& m)
{
    if (this == &m) {
        return *this;
    }

    for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			data_[i][j] = m.data_[i][j];
		}
    }

    return *this;
}

INLINE void Matrix4x4::GetData(float* data) const {
	int idx = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			data[idx++] = data_[j][i];
		}
	}
}

}

#endif
