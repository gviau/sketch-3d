#ifndef SKETCH_3D_VECTOR_3_H
#define SKETCH_3D_VECTOR_3_H

#include "Common.h"

namespace Sketch3D {

// Forward dependencies
class Matrix3x3;

/**
 * @class Vector3
 * The Vector3 class represents a 3D vector class.
 */
class Vector3 {
    public:
        float                   x;    /**< X component */
        float                   y;    /**< Y component */
        float                   z;    /**< Z component */

        // USEFUL STATICS

        static const Vector3    ZERO;   /**< Zero vector */
        static const Vector3    ONE;    /**< One vector */
        static const Vector3    RIGHT;  /**< Only the x component is set to 1 */
        static const Vector3    UP;     /**< Only the y component is set to 1 */
        static const Vector3    LOOK;   /**< Only the z component is set to 1 */

        /**
         * Default constructor. Initializes everything to 0.
         */
                                Vector3();

        /**
         * Constructor.
         * @param x The x component
         * @param y The y component
         * @param z The z component
         */
                                Vector3(float x, float y, float z);

        /**
         * Copy constructor
         * @param src The src vector that will be copied
         */
                                Vector3(const Vector3& src);

        /**
         * Returns the length of the vector. It should be noted that the length
         * is a costly operation since it requires a square root. You may want
         * to check the SquaredLength function.
         */
        float                   Length() const;

        /**
         * Returns the squared length of the vector.
         */
        float                   SquaredLength() const;

        /**
         * Return the normalized vector
         */
        Vector3                 Normalized() const;

        /**
         * Inline normalization of the vector
         */
        void                    Normalize();
    
        /**
         * Computes the dot product between this vector and v.
         * @param v The vector v with which the dot product will be computed
         * @return the dot product
         */
        float                   Dot(const Vector3& v) const;

        /**
         * Computes the cross product between this vector and v.
         * @param v The vector v with which to compute the cross product
         * @return a new vector perpendicular to this one and v
         */
        Vector3                 Cross(const Vector3& v) const;

        // UNARY OPERATORS
        /**
         * Returns the negated vector
         */
        INLINE Vector3          operator-() const;

        // BINARY OPERATORS
        INLINE Vector3          operator+(float f) const;
        INLINE Vector3          operator+(const Vector3& v) const;
        INLINE Vector3          operator-(float f) const;
        INLINE Vector3          operator-(const Vector3& v) const;
        INLINE Vector3          operator*(float f) const;
        INLINE Vector3          operator*(const Matrix3x3& m) const;
        INLINE Vector3          operator/(float f) const;

        INLINE friend Vector3   operator+(float f, const Vector3& v);
        INLINE friend Vector3   operator-(float f, const Vector3& v);
        INLINE friend Vector3   operator*(float f, const Vector3& v);

        INLINE void             operator+=(float f);
        INLINE void             operator+=(const Vector3& v);
        INLINE void             operator-=(float f);
        INLINE void             operator-=(const Vector3& v);
        INLINE void             operator*=(float f);
        INLINE void             operator*=(const Matrix3x3& m);
        INLINE void             operator/=(float f);

        INLINE bool             operator==(const Vector3& v) const;
        INLINE bool             operator!=(const Vector3& v) const;

        INLINE Vector3&         operator=(const Vector3& v);

};

}

#endif
