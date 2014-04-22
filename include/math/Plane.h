#ifndef SKETCH_3D_PLANE_H
#define SKETCH_3D_PLANE_H

#include "system/common.h"
#include "math/vector3.h"

namespace Sketch3D
{
/**
 * @class Plane
 * Represents an infinite plane defined by its normal and its distance from the
 * origin.
 */
class Plane
{
    public:
        /**
         * Default constructor. 
         * Defaults to the origin with the plane pointing in the up direction
         */
                                Plane();

        /**
         * Constructor
         * @param normal The normal vector. It does not need to be normalized
         * since the constructor will normalize it.
         * @param distance The distance from the origin
         */
                                Plane(const Vector3& normal, float distance);

        /**
         * Constructor. 
         * The normal components don't need to be normalized since the
         * Constructor will normalze the vector.
         * @param nx The x normal's component
         * @param ny The y normal's component
         * @param nz The z normal's component
         * @param distance The plane's distance
         */
                                Plane(float nx, float ny, float nz,
                                      float distance);

        /**
         * Constructor.
         * Constructs the plane from 3 points
         * @param p1 The first point
         * @param p2 The second point
         * @param p3 The third point
         */
                                Plane(const Vector3& p1, const Vector3& p2,
                                      const Vector3& p3);

        /**
         * The normal will be normalized by the function
         */
        INLINE void             SetNormal(const Vector3& normal);

        /**
         * The normal will be normalized by the function
         */
        INLINE void             SetNormal(float nx, float ny, float nz);

        /**
         * Sets a new normal, but don't normalize it. It assumes it is
         * normalized by the caller.
         */
        INLINE void             SetNormalizedNormal(const Vector3& normal);

        INLINE void             SetDistance(float distance);

        INLINE const Vector3&   GetNormal() const;
        INLINE float            GetDistance() const;

    private:
        Vector3                 normal_;    /**< The plane's normal (A, B, C parameters) */
        float                   distance_;  /**< The plane's distance (D parameter) */
};

INLINE void Plane::SetNormal(const Vector3& normal)
{
    normal_ = normal.Normalized();
}

INLINE void Plane::SetNormal(float nx, float ny, float nz)
{
    normal_ = Vector3(nx, ny, nz);
    normal_.Normalize();
}

INLINE void Plane::SetNormalizedNormal(const Vector3& normal)
{
    normal_ = normal;
}

INLINE void Plane::SetDistance(float distance)
{
    distance_ = distance;
}

INLINE const Vector3& Plane::GetNormal() const
{
    return normal_;
}

INLINE float Plane::GetDistance() const
{
    return distance_;
}

}

#endif
