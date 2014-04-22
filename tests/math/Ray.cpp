#include <boost/test/unit_test.hpp>

#include "math/Plane.h"
#include "math/Ray.h"

using namespace Sketch3D;

BOOST_AUTO_TEST_CASE(test_plane_intersection)
{
    Ray ray(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 0.0f));
    Plane plane(Vector3::UP, 10.0f);

    float t;
    bool val = ray.IntersectsPlane(plane, &t);

    BOOST_CHECK(val == true);
}
