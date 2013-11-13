#ifndef SKETCH_3D_MATRIX_3X3_H
#define SKETCH_3D_MATRIX_3X3_H

#include "Common.h"

namespace Sketch3D
{

// Forward dependencies
class Vector3;

/**
* @class Matrix3x3
* Represents a 3x3 matrix.
*/
class Matrix3x3
{
    public:
        // USEFUL STATICS
        static const Matrix3x3  IDENTITY;   /**< Only the main diagonal is set to 1 */
        static const Matrix3x3  ZERO; /**< Everything to zero */
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
        INLINE Matrix3x3        operator-() const;

        // BINARY OPERATORS
        INLINE Matrix3x3        operator*(const Matrix3x3& m) const;
        INLINE void             operator*=(const Matrix3x3& m);

        INLINE bool             operator==(const Matrix3x3& m) const;
        INLINE bool             operator!=(const Matrix3x3& m) const;

        INLINE Matrix3x3&       operator=(const Matrix3x3& m);

    private:
        float                   data_[9];   /**< The matrix represented as a linear array */
};

}

#endif
