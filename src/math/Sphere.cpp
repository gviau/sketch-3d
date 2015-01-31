#include "math/Sphere.h"

#include "math/Ray.h"

namespace Sketch3D {
Sphere::Sphere() : radius_(1.0f) {
}

Sphere::Sphere(const Vector3& center, float radius) : center_(center), radius_(radius) {
}

RelativePlanePosition_t Sphere::IntersectsPlane(const Plane& plane) const {
    float distance = plane.GetNormal().Dot(center_) + plane.GetDistance();

    if (distance < -radius_) {
        return RELATIVE_PLANE_POSITION_OUTSIDE;
    }

    if (abs(distance) <= radius_) {
        return RELATIVE_PLANE_POSITION_INTERSECTS;
    }

    return RELATIVE_PLANE_POSITION_INSIDE;
}

bool Sphere::IntersectsRay(const Ray& ray, float* t) const {
    Vector3 pos = ray.GetOrigin() - center_;
    float b = -ray.GetDirection().Dot(pos);
    float c = pos.Dot(pos) - radius_ * radius_;

    float d = b*b - c;
    if (d < 0.0f) {
        return false;
    }

    d = sqrtf(d);
    float t0 = b + d;
    float t1 = b - d;

    if (t1 > EPSILON) {
        if (t != nullptr) {
            *t = t1;
        }

        if (t0 > EPSILON && t0 < t1) {
            if (t != nullptr) {
                *t = t0;
            }
        }

        return true;
    }

    return false;
}

void Sphere::SetCenter(const Vector3& center) {
    center_ = center;
}

void Sphere::SetRadius(float radius) {
    radius_ = radius;
}

const Vector3& Sphere::GetCenter() const {
    return center_;
}

float Sphere::GetRadius() const {
    return radius_;
}
}