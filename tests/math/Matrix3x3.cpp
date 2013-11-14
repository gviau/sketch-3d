#include <boost/test/unit_test.hpp>

#include "Constants.h"
#include "Matrix3x3.h"
#include "Vector3.h"

using namespace Sketch3D;

BOOST_AUTO_TEST_CASE(test_3x3_transpose)
{
    Matrix3x3 m = Matrix3x3::IDENTITY;
    BOOST_REQUIRE(m.Transpose() == m);

    m(1, 0) = 5.0f;
    m(2, 0) = 10.0f;

    Matrix3x3 t(1.0f, 0.0f, 0.0f,
                5.0f, 1.0f, 0.0f,
                10.0f, 0.0f, 1.0f);
    BOOST_REQUIRE(m.Transpose() == t);
}

BOOST_AUTO_TEST_CASE(test_3x3_rotation_around_x)
{
    Matrix3x3 m;
    Vector3 v = Vector3::UP;
    m.RotationAroundX(PI);
    v *= m;
    BOOST_REQUIRE(v == -Vector3::UP);

    v = Vector3::ONE;
    v = m * v;
    BOOST_REQUIRE(v == Vector3(1.0f, -1.0f, -1.0f));
}

BOOST_AUTO_TEST_CASE(test_3x3_rotation_around_y)
{
    Matrix3x3 m;
    Vector3 v = Vector3::LOOK;
    m.RotationAroundY(PI);
    v *= m;
    BOOST_REQUIRE(v == -Vector3::LOOK);

    v = Vector3::ONE;
    v = m * v;
    BOOST_REQUIRE(v == Vector3(-1.0f, 1.0f, -1.0f));
}

BOOST_AUTO_TEST_CASE(test_3x3_rotation_around_z)
{
    Matrix3x3 m;
    Vector3 v = Vector3::RIGHT;
    m.RotationAroundZ(PI);
    v *= m;
    BOOST_REQUIRE(v == -Vector3::RIGHT);

    v = Vector3::ONE;
    v = m * v;
    BOOST_REQUIRE(v == Vector3(-1.0f, -1.0f, 1.0f));
}

BOOST_AUTO_TEST_CASE(test_3x3_multiple_rotations)
{
    Matrix3x3 m, n, o;
    m.RotationAroundX(PI_OVER_2);
    n.RotationAroundY(40 * DEG_2_RAD);
    o.RotationAroundZ(310 * DEG_2_RAD);

    Matrix3x3 rot = m * n * o;

    Vector3 v = Vector3::ONE;
    Vector3 w = v * rot;

    BOOST_REQUIRE(w == Vector3(-0.737208f, 0.686816f, -1.40883f));
}

BOOST_AUTO_TEST_CASE(test_3x3_rotate_around_axis)
{
}

BOOST_AUTO_TEST_CASE(test_3x3_matrix_matrix_multiply)
{
    Matrix3x3 i = Matrix3x3::IDENTITY;
    Matrix3x3 m(1.0f, 2.0f, 3.0f,
                2.0f, 4.0f, 6.0f, 
                3.0f, 6.0f, 9.0f);
    BOOST_REQUIRE(i * m == m);
    BOOST_REQUIRE(m * i == m);

    Matrix3x3 n(9.0f, 6.0f, 3.0f,
                6.0f, 4.0f, 2.0f,
                3.0f, 2.0f, 1.0f);
    Matrix3x3 o = m * n;
    n *= m;
    
    Matrix3x3 r1(30.0f, 20.0f, 10.0f,
                 60.0f, 40.0f, 20.0f,
                 90.0f, 60.0f, 30.0f);
    Matrix3x3 r2(30.0f, 60.0f, 90.0f,
                 20.0f, 40.0f, 60.0f,
                 10.0f, 20.0f, 30.0f);

    BOOST_REQUIRE(o == r1);
    BOOST_REQUIRE(n == r2);

}   
