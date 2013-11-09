#ifndef SKETCH_3D_RAY_H
#define SKETCH_3D_RAY_H

#include "Common.h"
#include "Vector3.h"

namespace Sketch3D
{

// Forward class declaration
class Plane;

/**
* @class Ray
* Represents a ray in space as an origin and a unit length direction vector.
*/
class Ray
{
    public:
        /**
        * Constructor
        * @param origin A 3D vector representing the origin of the ray
        * @param direction A 3D vector representing the direction of the ray.
        * It is not necessary for it to be normalized since the constructor
        * will normalize it.
        */
                                Ray(const Vector3& origin,
                                    const Vector3& direction);

        /**
        * Constructor. The direction will be normalized inside the constructor.
        * @param ox The x position of the origin
        * @param oy The y position of the origin
        * @param oz The z position of the origin
        * @param dx The x direction
        * @param dy The y direction
        * @param dz The z direction
        */
                                Ray(float ox, float oy, float oz,
                                    float dx, float dy, float dz);

        /**
        * Determines if the ray intersects a Plane
        * @param plane The plane to check for the intersection
        * @param t A pointer to a float. If not NULL, then the float will have
        * the distance from the origin of the ray at which the intersection
        * occured.
        * @return True if the ray intersected the plane, false otherwise.
        */
        bool                    IntersectsPlane(const Plane& plane,
                                                float* t) const;
        
        INLINE void             SetOrigin(const Vector3& origin);
        INLINE void             SetOrigin(float ox, float oy, float oz);

        /**
        * The direction will be normalized by the function
        */
        INLINE void             SetDirection(const Vector3& direction);

        /**
        * The direction will be normalized by the function
        */
        INLINE void             SetDirection(float dx, float dy, float dz);

        /**
        * Sets a new direction, but don't normalize it. It assumes it is
        * normalized by the caller.
        * @param direction The normalized vector to use as a new direction
        */
        INLINE void             SetNormalizedDirection(const Vector3& direction);

        INLINE const Vector3&   GetOrigin() const;
        INLINE const Vector3&   GetDirection() const;

    private:
        Vector3                 origin_;     /**< The origin of the ray */
        Vector3                 direction_;  /**< The direction of the ray */
};
}

#endif
