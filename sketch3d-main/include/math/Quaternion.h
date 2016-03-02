#ifndef SKETCH_3D_QUATERNION_H
#define SKETCH_3D_QUATERNION_H

#include "math/Matrix3x3.h"
#include "math/Matrix4x4.h"

#include "system/Platform.h"

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
class SKETCH_3D_API Quaternion
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
         * Computes the dot product between two quaternions
         * @param q The quaternion with which to compute the dot product
         */
        float                   Dot(const Quaternion& q) const;

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
         * Sets the Quaternion parts from Euler angles.
         * @param pitchInRadians The pitch angle in radians
         * @param yawInRadians The yaw angle in radians
         * @param rollInRadians The roll angle in radians
         */
        void                    MakeFromEulerAngles(float pitchInRadians, float yawInRadians, float rollInRadians);

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
         * Linearly interpolate between this quaternion and another specified quaternion that represents
         * the end value
         * @param endQuat The end quaternion value for the interpolation
         * @param t The interpolation factor
         * @param shortestPath Use the shortest path between the two quaternions
         * @return The new interpolated quaternion
         */
        Quaternion              Slerp(const Quaternion& endQuat, float t, bool shortestPath=false) const;

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

        // UNARY OPERATORS
        Quaternion              operator-() const;

        // BINARY OPERATORS
        INLINE Quaternion       operator+(const Quaternion& q) const;
        Quaternion              operator*(const Quaternion& q) const;
        INLINE Quaternion       operator*(float f) const;

        void                    operator*=(const Quaternion& q);

        bool                    operator==(const Quaternion& q) const;
        bool                    operator!=(const Quaternion& q) const;
};

INLINE Quaternion Quaternion::operator+(const Quaternion& q) const {
    return Quaternion(w+q.w, x+q.x, y+q.y, z+q.z);
}

INLINE Quaternion Quaternion::operator*(float f) const {
    return Quaternion(w*f, x*f, y*f, z*f);
}

}

#endif
