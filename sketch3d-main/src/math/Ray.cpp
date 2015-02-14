#include "math/Ray.h"

#include "math/Constants.h"
#include "math/Plane.h"

namespace Sketch3D
{
Ray::Ray(const Vector3& origin, const Vector3& direction) :
    origin_(origin), direction_(direction)
{
    direction_.Normalize();
}

Ray::Ray(float ox, float oy, float oz, float dx, float dy, float dz) :
    origin_(Vector3(ox, oy, oz)), direction_(Vector3(dx, dy, dz))
{
    direction_.Normalize();
}

bool Ray::IntersectsPlane(const Plane& plane, float* t) const
{
    // Are the direction vector and normal of the plane perpendicular?
    float dot = direction_.Dot(plane.GetNormal());
    if (dot <= EPSILON) {
        return false;
    }

    // Get the distance if we asked it
    if (t) {
        *t = -(origin_.Dot(plane.GetNormal()) + plane.GetDistance()) / dot;
    }

    return true;
}

}
