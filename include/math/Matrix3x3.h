#ifndef SKETCH_3D_MATRIX_3X3_H
#define SKETCH_3D_MATRIX_3X3_H

#include "system/Common.h"

namespace Sketch3D
{

// Forward dependencies
class Vector3;

/**
 * @class Matrix3x3
 * Represents a column-major 3x3 matrix.
 */
class Matrix3x3
{
	friend class ShaderOpenGL;

    public:
        // USEFUL STATICS
        static const Matrix3x3  IDENTITY;   /**< Only the main diagonal is set to 1 */
        static const Matrix3x3  ZERO;       /**< Everything to zero */
        /**
         * Default constructor. Initializes everything to 0.
         */
                                Matrix3x3();

        /**
         * Constructor. Parameters are entered row by row.
         */
                                Matrix3x3(float m11, float m12, float m13,
                                          float m21, float m22, float m23,
                                          float m31, float m32, float m33);

        /**
         * Constructor
         * @param data An array of 9 floats to populate the matrix
         */
                                Matrix3x3(float* data);

        /**
         * Copy constructor
         * @param src The matrix to copy
         */
                                Matrix3x3(const Matrix3x3& src);

        /**
         * Return the transpose this matrix
         */
        Matrix3x3               Transpose() const;

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
        INLINE Matrix3x3        operator-() const;

        // BINARY OPERATORS
        INLINE Matrix3x3        operator*(const Matrix3x3& m) const;
        Vector3					operator*(const Vector3& v) const;
        INLINE void             operator*=(const Matrix3x3& m);

        INLINE bool             operator==(const Matrix3x3& m) const;
        INLINE bool             operator!=(const Matrix3x3& m) const;

        INLINE Matrix3x3&       operator=(const Matrix3x3& m);

		/**
		 * Return the matrix as a column-major linear array.
		 * @param The returned array
		 */
		INLINE void				GetData(float* data) const;

    private:
		float                   data_[3][3];	/**< The matrix represented as a 2D array */
};

INLINE const float* Matrix3x3::operator[] (int row) const
{
    return data_[row];
}

INLINE float* Matrix3x3::operator[] (int row)
{
    return data_[row];
}

INLINE Matrix3x3 Matrix3x3::operator-() const
{
    float data[16];
	int idx = 0;
    for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			data[idx] = -data_[i][j];
			idx += 1;
		}
    }

    return Matrix3x3(data);
}

INLINE Matrix3x3 Matrix3x3::operator*(const Matrix3x3& m) const
{
    Matrix3x3 mat = ZERO;
    mat.data_[0][0] = data_[0][0] * m.data_[0][0] + data_[0][1] * m.data_[1][0] + data_[0][2] * m.data_[2][0];
    mat.data_[0][1] = data_[0][0] * m.data_[0][1] + data_[0][1] * m.data_[1][1] + data_[0][2] * m.data_[2][1];
    mat.data_[0][2] = data_[0][0] * m.data_[0][2] + data_[0][1] * m.data_[1][2] + data_[0][2] * m.data_[2][2];

    mat.data_[1][0] = data_[1][0] * m.data_[0][0] + data_[1][1] * m.data_[1][0] + data_[1][2] * m.data_[2][0];
    mat.data_[1][1] = data_[1][0] * m.data_[0][1] + data_[1][1] * m.data_[1][1] + data_[1][2] * m.data_[2][1];
    mat.data_[1][2] = data_[1][0] * m.data_[0][2] + data_[1][1] * m.data_[1][2] + data_[1][2] * m.data_[2][2];

    mat.data_[2][0] = data_[2][0] * m.data_[0][0] + data_[2][1] * m.data_[1][0] + data_[2][2] * m.data_[2][0];
    mat.data_[2][1] = data_[2][0] * m.data_[0][1] + data_[2][1] * m.data_[1][1] + data_[2][2] * m.data_[2][1];
    mat.data_[2][2] = data_[2][0] * m.data_[0][2] + data_[2][1] * m.data_[1][2] + data_[2][2] * m.data_[2][2];

    return mat;
}

INLINE void Matrix3x3::operator*=(const Matrix3x3& m)
{
    Matrix3x3 mat = ZERO;
    mat.data_[0][0] = data_[0][0] * m.data_[0][0] + data_[0][1] * m.data_[1][0] + data_[0][2] * m.data_[2][0];
    mat.data_[0][1] = data_[0][0] * m.data_[0][1] + data_[0][1] * m.data_[1][1] + data_[0][2] * m.data_[2][1];
    mat.data_[0][2] = data_[0][0] * m.data_[0][2] + data_[0][1] * m.data_[1][2] + data_[0][2] * m.data_[2][2];

    mat.data_[1][0] = data_[1][0] * m.data_[0][0] + data_[1][1] * m.data_[1][0] + data_[1][2] * m.data_[2][0];
    mat.data_[1][1] = data_[1][0] * m.data_[0][1] + data_[1][1] * m.data_[1][1] + data_[1][2] * m.data_[2][1];
    mat.data_[1][2] = data_[1][0] * m.data_[0][2] + data_[1][1] * m.data_[1][2] + data_[1][2] * m.data_[2][2];

    mat.data_[2][0] = data_[2][0] * m.data_[0][0] + data_[2][1] * m.data_[1][0] + data_[2][2] * m.data_[2][0];
    mat.data_[2][1] = data_[2][0] * m.data_[0][1] + data_[2][1] * m.data_[1][1] + data_[2][2] * m.data_[2][1];
    mat.data_[2][2] = data_[2][0] * m.data_[0][2] + data_[2][1] * m.data_[1][2] + data_[2][2] * m.data_[2][2];

    (*this) = mat;
}

INLINE bool Matrix3x3::operator==(const Matrix3x3& m) const
{
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (fabs(data_[i][j] - m.data_[i][j]) > EPSILON) {
				return false;
			}
		}
	}

    return true;
}

INLINE bool Matrix3x3::operator!=(const Matrix3x3& m) const
{
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (fabs(data_[i][j] - m.data_[i][j]) > EPSILON) {
				return true;
			}
		}
	}

    return false;
}

INLINE Matrix3x3& Matrix3x3::operator=(const Matrix3x3& m)
{
    if (this == &m) {
        return *this;
    }

    for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			data_[i][j] = m.data_[i][j];
		}
    }

    return *this;
}

INLINE void Matrix3x3::GetData(float* data) const {
	int idx = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			data[idx++] = data_[j][i];
		}
	}
}

}

#endif
