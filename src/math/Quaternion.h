#ifndef SKETCH_3D_QUATERNION_H
#define SKETCH_3D_QUATERNION_H

#include "Matrix3x3.h"
#include "Matrix4x4.h"

namespace Sketch3D {

// Forward class declaration
class Vector3;

/**
* @class Quaternion
* Represents a Quaternion, used to create rotation matrices. Its main purpose
* is to create rotations free of gimbal locks. A quaternion is represented by a
* real part and 3 imaginary part: w + xi + yj + zk
*
* Multiplications of Quaternions are used to combine rotations.
*/
class Quaternion
{
    public:
        float                   w;   /**< Real part */
        float                   x;   /**< i imaginary part */
        float                   y;   /**< j imaginary part */
        float                   z;   /**< k imaginary part */

        // USEFUL STATICS
        static const Quaternion IDENTITY;   /**< w is set to 1, the rest is 0 */
        static const Quaternion ZERO;       /**< Everything to 0 */

        /**
        * Default constructor. Initializes the quaternion to its identity.
        */
                                Quaternion();

        /**
        * Initializes the Quaternion to set values
        * @param w The real part w
        * @param x i imaginary part
        * @param y j imaginary part
        * @param z k imaginary part
        */
                                Quaternion(float w, float x, float y, float z);

        /**
        * Returns the magnitude of the quaternion
        */
        float                   Magnitude() const;

        /**
        * Returns the normalized quaternion
        */
        Quaternion              Normalized() const;

        /**
        * Inline normalization
        */
        void                    Normalize();

        /**
        * Sets the Quaternion parts from a rotation matrix.
        * @param mat The rotation matrix to use to set the quaternion
        */
        void                    MakeFromRotationMatrix(const Matrix3x3& mat);

        /**
        * Sets the Quaternion parts from a rotation matrix.
        * @param mat The rotation matrix to use to set the quaternion
        */
        void                    MakeFromRotationMatrix(const Matrix4x4& mat);

        /**
        * Sets the Quaternion parts from an angle and an axis. The axis is
        * expected to be normalized.
        * @param angle The angle in radian to rotate around the axis
        * @param axis A normalized Vector3 object
        */
        void                    MakeFromAngleAxis(float angle,
                                                  const Vector3& axis);

        /**
        * Sets the Quaternion parts from 3 axes. The axes are expected to be
        * normalized.
        * @param xAxis The first axis representing the X axis
        * @param yAxis The second axis representing the Y axis
        * @param zAxis The third axis representing the Z axis
        */
        void                    MakeFromAxes(const Vector3& xAxis,
                                             const Vector3& yAxis,
                                             const Vector3& zAxis);

        /** 
        * Constructs the rotation matrix from the quaternion.
        * @parm mat The 3x3 matrix to construct
        */
        void                    ToRotationMatrix(Matrix3x3& mat) const;

        /** 
        * Constructs the rotation matrix from the quaternion.
        * @parm mat The 4x4 matrix to construct
        */
        void                    ToRotationMatrix(Matrix4x4& mat) const;

        /**
        * Derives the Quaternion to return an angle and an axis
        * @param angle The returned angle in radians
        * @param axis The returned axis
        */
        void                    ToAngleAxis(float& angle, Vector3& axis) const;

        /**
        * Derives the Quaternion to return the 3 axes that composes it.
        * @param xAxis The first axis representing the X axis
        * @param yAxis The second axis representing the Y axis
        * @param zAxis The third axis representing the Z axis
        */
        void                    ToAxes(Vector3& xAxis, Vector3& yAxis,
                                       Vector3& zAxis) const;

        /**
        * Returns the x axis from this quaternion
        */
        Vector3                 GetXAxis() const;

        /**
        * Returns the y axis from this quaternion
        */
        Vector3                 GetYAxis() const;

        /**
        * Returns the z axis from this quaternion
        */
        Vector3                 GetZAxis() const;

        // BINARY OPERATORS
        Quaternion              operator*(const Quaternion& q) const;
        void                    operator*=(const Quaternion& q);

        bool                    operator==(const Quaternion& q) const;
        bool                    operator!=(const Quaternion& q) const;
};

}

#endif
