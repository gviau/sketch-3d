#include "Ray.h"

#include "Constants.h"
#include "Plane.h"

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

INLINE void Ray::SetOrigin(const Vector3& origin)
{
    origin_ = origin;
}

INLINE void Ray::SetOrigin(float ox, float oy, float oz)
{
    origin_ = Vector3(ox, oy, oz);
}

INLINE void Ray::SetDirection(const Vector3& direction)
{
    direction_ = direction.Normalized();
}

INLINE void Ray::SetDirection(float dx, float dy, float dz)
{
    direction_ = Vector3(dx, dy, dz);
    direction_.Normalize();
}

INLINE void Ray::SetNormalizedDirection(const Vector3& direction)
{
    direction_ = direction;
}

INLINE const Vector3& Ray::GetOrigin() const
{
    return origin_;
}

INLINE const Vector3& Ray::GetDirection() const
{
    return direction_;
}

}
