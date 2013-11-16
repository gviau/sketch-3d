#ifndef SKETCH_3D_QUATERNION_H
#define SKETCH_3D_QUATERNION_H

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
        float       w;   /**< Real part */
        float       x;   /**< i imaginary part */
        float       y;   /**< j imaginary part */
        float       z;   /**< k imaginary part */
        /**
        * Default constructor. Initializes the quaternion to its identy.
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
        float       Magnitude() const;

        /**
        * Returns the normalized quaternion
        */
        Quaternion  Normalized() const;

        /**
        * Inline normalization
        */
        void        Normalize();

        /** 
        * Constructs the rotation matrix from the quaternion.
        * @parm mat The 4x4 matrix to construct
        */
        void        ConstructRotationMatrix(Matrix4x4& mat) const;
    
        /**
        * Sets the Quaternion parts from an angle and an axis. The axis is
        * expected to be normalized.
        * @param angle The angle in radian to rotate around the axis
        * @param axis A normalized Vector3 object
        */
        void        MakeFromAngleAxis(float angle, const Vector3& axis);

        // BINARY OPERATORS
        Quaternion  operator*(const Quaternion& q) const;
        void        operator*=(const Quaternion& q);

        bool        operator==(const Quaternion& q) const;
        bool        operator!=(const Quaternion& q) const;
};

}

#endif
