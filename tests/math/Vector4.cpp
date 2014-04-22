#include <boost/test/unit_test.hpp>

#include "math/Constants.h"
#include "math/Matrix4x4.h"
#include "math/Vector4.h"

using namespace Sketch3D;

// We use a greater epsilon for the tests because it SSE is enabled, some
// computations, such as reciprocal square roots, might be a bit less precise.
static bool CompareTo(float a, float b)
{
    return fabs(a - b) <= 0.001f;
}

BOOST_AUTO_TEST_CASE(test_vector4_dot_product)
{
    Vector4 u(5.0f, 10.0f, 0.0f);
    Vector4 v(1.0f, 1.0f, 1.0f);
    float dot = u.Dot(v);
    BOOST_REQUIRE(dot == 15.0f);
}

BOOST_AUTO_TEST_CASE(test_vector4_cross_product)
{
    Vector4 u = Vector4::RIGHT;
    Vector4 v = Vector4::UP;
    Vector4 w = u.Cross(v);

    BOOST_REQUIRE(w == Vector4::LOOK);
    BOOST_REQUIRE(v.Cross(u) != Vector4::LOOK);
}

BOOST_AUTO_TEST_CASE(test_vector4_length)
{
    Vector4 v(3.0f, 4.0f, 5.0f);
    float expectedSqrLength = 3.0f * 3.0f + 4.0f * 4.0f + 5.0f * 5.0f;
    float expectedLength = sqrtf(expectedSqrLength);
    float sqrLength = v.SquaredLength();
    float length = v.Length();

    BOOST_REQUIRE(CompareTo(sqrLength, expectedSqrLength));
    BOOST_REQUIRE(CompareTo(length, expectedLength));
}

BOOST_AUTO_TEST_CASE(test_vector4_normalize)
{
    Vector4 v(3.0f, 4.0f, 5.0f);
    Vector4 u = v.Normalized();
    v.Normalize();
    Vector4 w(0.4242640687119285f,
              0.565685424949238f,
              0.7071067811865475f);

    BOOST_REQUIRE(CompareTo(u.x, w.x) && CompareTo(u.y, w.y) &&
                  CompareTo(u.z, w.z));
    BOOST_REQUIRE(CompareTo(v.x, w.x) && CompareTo(v.y, w.y) &&
                  CompareTo(v.z, w.z));
}

BOOST_AUTO_TEST_CASE(test_vector4_matrix_multiply)
{
    Vector4 v = Vector4::ONE;
    Matrix4x4 m = Matrix4x4::IDENTITY;

    BOOST_REQUIRE(v * m == Vector4::ONE);

    m(1, 0) = 5.0f;
    m(2, 0) = 10.0f;

    v *= m;

    BOOST_REQUIRE(v == Vector4(16.0f, 1.0f, 1.0f));

    m(1, 2) = 10.0f;

    v *= m;

    BOOST_REQUIRE(v == Vector4(31.0f, 1.0f, 11.0f));
}

