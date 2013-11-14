#include <boost/test/unit_test.hpp>

#include "Constants.h"
#include "Vector3.h"
#include "Matrix3x3.h"

using namespace Sketch3D;

static bool CompareTo(float a, float b)
{
    return fabs(a - b) <= EPSILON;
}

BOOST_AUTO_TEST_CASE(test_vector3_dot_product)
{
    Vector3 u(5.0f, 10.0f, 0.0f);
    Vector3 v(1.0f, 1.0f, 1.0f);
    float dot = u.Dot(v);
    BOOST_REQUIRE(dot == 15.0f);
}

BOOST_AUTO_TEST_CASE(test_vector3_cross_product)
{
    Vector3 u = Vector3::RIGHT;
    Vector3 v = Vector3::UP;
    Vector3 w = u.Cross(v);

    BOOST_REQUIRE(w == Vector3::LOOK);
    BOOST_REQUIRE(v.Cross(u) != Vector3::LOOK);
}

BOOST_AUTO_TEST_CASE(test_vector3_length)
{
    Vector3 v(3.0f, 4.0f, 5.0f);
    float expectedSqrLength = 3.0f * 3.0f + 4.0f * 4.0f + 5.0f * 5.0f;
    float expectedLength = sqrtf(expectedSqrLength);
    float sqrLength = v.SquaredLength();
    float length = v.Length();

    BOOST_REQUIRE(CompareTo(sqrLength, expectedSqrLength));
    BOOST_REQUIRE(CompareTo(length, expectedLength));
}

BOOST_AUTO_TEST_CASE(test_vector3_normalize)
{
    Vector3 v(3.0f, 4.0f, 5.0f);
    Vector3 u = v.Normalized();
    v.Normalize();
    Vector3 w(0.4242640687119285f,
              0.565685424949238f,
              0.7071067811865475f);
    BOOST_REQUIRE(u == w);
    BOOST_REQUIRE(v == w);
}

BOOST_AUTO_TEST_CASE(test_vector3_matrix_multiply)
{
    Vector3 v = Vector3::ONE;
    Matrix3x3 m = Matrix3x3::IDENTITY;

    BOOST_REQUIRE(v * m == Vector3::ONE);

    m(1, 0) = 5.0f;
    m(2, 0) = 10.0f;

    v *= m;

    BOOST_REQUIRE(v == Vector3(16.0f, 1.0f, 1.0f));

    m(1, 2) = 10.0f;

    v *= m;

    BOOST_REQUIRE(v == Vector3(31.0f, 1.0f, 11.0f));
}
