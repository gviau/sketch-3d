#ifndef SKETCH_3D_SPHERE_H
#define SKETCH_3D_SPHERE_H

#include "math/Plane.h"
#include "math/Vector3.h"

#include "system/Platform.h"

namespace Sketch3D {
// Forward declaration
class Ray;

/**
 * @class Sphere
 * This class represents a sphere.
 */
class SKETCH_3D_API Sphere {
    public:
        /**
         * Default constructor. Initializes a unit sphere at the origin
         */
                                Sphere();

        /**
         * Constructor.
         * @param center The center of the sphere
         * @param radius The radius of the sphere
         */
                                Sphere(const Vector3& center, float radius);

        /**
         * Check if the given ray intersects with the sphere
         * @param ray The ray with which to check the intersection
         * @param t A pointer to a float. If no null, then it will have the distance at which the ray intersects with the sphere
         * @return true if an intersection exists, false otherwise
         */
        bool                    IntersectsRay(const Ray& ray, float* t) const;

        /**
         * Check if the given plane intersects with the sphere
         * @param plane The plane with which to check the intersection
         * @return The relative position of the object in respect to the plane
         */
        RelativePlanePosition_t IntersectsPlane(const Plane& plane) const;

        void                    SetCenter(const Vector3& center);
        void                    SetRadius(float radius);
        const Vector3&          GetCenter() const;
        float                   GetRadius() const;
    private:
        Vector3                 center_;    /**< Center of the sphere */
        float                   radius_;    /**< Radius of the sphere */
};

}

#endif