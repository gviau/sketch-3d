#include <boost/test/unit_test.hpp>

#include "math/Constants.h"
#include "math/Matrix4x4.h"
#include "math/Vector4.h"

using namespace Sketch3D;

BOOST_AUTO_TEST_CASE(test_4x4_transpose)
{
    Matrix4x4 m = Matrix4x4::IDENTITY;
    BOOST_REQUIRE(m.Transpose() == m);

    m[0][1] = 5.0f;
    m[0][2] = 10.0f;
    m[0][3] = 15.0f;

    Matrix4x4 t(1.0f, 0.0f, 0.0f, 0.0f,
                5.0f, 1.0f, 0.0f, 0.0f,
                10.0f, 0.0f, 1.0f, 0.0f,
                15.0f, 0.0f, 0.0f, 1.0f);
    BOOST_REQUIRE(m.Transpose() == t);
}

BOOST_AUTO_TEST_CASE(test_4x4_rotation_around_x)
{
    Matrix4x4 m;
    Vector4 v = Vector4::UP;
    m.RotationAroundX(PI);
    v *= m;
    BOOST_REQUIRE(v == -Vector4::UP);

    v = Vector4::ONE;
    v = m * v;
    BOOST_REQUIRE(v == Vector4(1.0f, -1.0f, -1.0f));
}

BOOST_AUTO_TEST_CASE(test_4x4_rotation_around_y)
{
    Matrix4x4 m;
    Vector4 v = Vector4::LOOK;
    m.RotationAroundY(PI);
    v *= m;
    BOOST_REQUIRE(v == -Vector4::LOOK);

    v = Vector4::ONE;
    v = m * v;
    BOOST_REQUIRE(v == Vector4(-1.0f, 1.0f, -1.0f));
}

BOOST_AUTO_TEST_CASE(test_4x4_rotation_around_z)
{
    Matrix4x4 m;
    Vector4 v = Vector4::RIGHT;
    m.RotationAroundZ(PI);
    v *= m;
    BOOST_REQUIRE(v == -Vector4::RIGHT);

    v = Vector4::ONE;
    v = m * v;
    BOOST_REQUIRE(v == Vector4(-1.0f, -1.0f, 1.0f));
}

BOOST_AUTO_TEST_CASE(test_4x4_multiple_rotations)
{
    Matrix4x4 m, n, o;
    m.RotationAroundX(PI_OVER_2);
    n.RotationAroundY(40 * DEG_2_RAD);
    o.RotationAroundZ(310 * DEG_2_RAD);

    Matrix4x4 rot = m * n * o;

    Vector4 v = Vector4::ONE;
    Vector4 w = v * rot;

    BOOST_REQUIRE(w == Vector4(-0.737208f, 0.686816f, -1.40883f));
}

BOOST_AUTO_TEST_CASE(test_4x4_rotate_around_axis)
{
}

BOOST_AUTO_TEST_CASE(test_4x4_matrix_matrix_multiply)
{
    Matrix4x4 i = Matrix4x4::IDENTITY;
    Matrix4x4 m(1.0f, 2.0f,  3.0f,  4.0f,
                2.0f, 4.0f,  6.0f,  8.0f,
                3.0f, 6.0f,  9.0f, 12.0f,
                4.0f, 8.0f, 12.0f, 16.0f);
    BOOST_REQUIRE(i * m == m);
    BOOST_REQUIRE(m * i == m);

    Matrix4x4 n(16.0f, 12.0f, 8.0f, 4.0f,
                12.0f, 9.0f,  6.0f, 3.0f,
                8.0f,  6.0f,  4.0f, 2.0f,
                4.0f,  3.0f,  2.0f, 1.0f);
    Matrix4x4 o = m * n;
    n *= m;
    
    Matrix4x4 r1( 80.0f,  60.0f,  40.0f, 20.0f,
                 160.0f, 120.0f,  80.0f, 40.0f,
                 240.0f, 180.0f, 120.0f, 60.0f,
                 320.0f, 240.0f, 160.0f, 80.0f);
    Matrix4x4 r2(80.0f, 160.0f, 240.0f, 320.0f,
                 60.0f, 120.0f, 180.0f, 240.0f,
                 40.0f,  80.0f, 120.0f, 160.0f,
                 20.0f,  40.0f,  60.0f,  80.0f);

    BOOST_REQUIRE(o == r1);
    BOOST_REQUIRE(n == r2);

}   

