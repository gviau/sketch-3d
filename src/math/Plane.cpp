#include "Plane.h"

namespace Sketch3D
{
Plane::Plane() : normal_(Vector3::UP), distance_(0.0f)
{
}

Plane::Plane(const Vector3& normal, float distance) :
    normal_(normal), distance_(distance)
{
    normal_.Normalize();
}

Plane::Plane(float nx, float ny, float nz, float distance):
    normal_(Vector3(nx, ny, nz)), distance_(distance)
{
    normal_.Normalize();
}

Plane::Plane(const Vector3& p1, const Vector3& p2, const Vector3& p3)
{
    Vector3 u = p2 - p1;
    Vector3 v = p3 - p1;

    normal_ = u.Cross(v).Normalized();

    distance_ = normal_.x * p1.x + normal_.y * p1.y + normal_.z * p1.z;
}

void Plane::SetNormal(const Vector3& normal)
{
    normal_ = normal.Normalized();
}

void Plane::SetNormal(float nx, float ny, float nz)
{
    normal_ = Vector3(nx, ny, nz);
    normal_.Normalize();
}

void Plane::SetNormalizedNormal(const Vector3& normal)
{
    normal_ = normal;
}

void Plane::SetDistance(float distance)
{
    distance_ = distance;
}

const Vector3& Plane::GetNormal() const
{
    return normal_;
}

float Plane::GetDistance() const
{
    return distance_;
}

}
