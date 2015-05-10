#include <boost/test/unit_test.hpp>

#include "math/Constants.h"
#include "math/Quaternion.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

using namespace Sketch3D;

#include <iostream>
using namespace std;

BOOST_AUTO_TEST_CASE(test_quaternion_construct_rotation)
{
    Quaternion q;
    Matrix4x4 m;
    q.ToRotationMatrix(m);

    BOOST_REQUIRE(m == Matrix4x4::IDENTITY);
}

BOOST_AUTO_TEST_CASE(test_quaternion_make_from_matrix_3x3)
{
    Matrix3x3 a, b, c;
    a.RotationAroundX(PI_OVER_2);
    b.RotationAroundZ(50 * DEG_2_RAD);
    c = a * b;

    Vector3 u = Vector3::ONE;
    Vector3 v = u * c;

    Quaternion q;
    q.MakeFromRotationMatrix(c);
    Matrix3x3 m;
    q.ToRotationMatrix(m);

    Vector3 w = u * m;

    BOOST_REQUIRE(v == w);
}

BOOST_AUTO_TEST_CASE(test_quaternion_make_from_matrix_4x4)
{
    Matrix4x4 a, b, c;
    a.RotationAroundY(83 * DEG_2_RAD);
    b.RotationAroundX(122 * DEG_2_RAD);
    c = a * b;

    Vector4 u = Vector4::ONE;
    Vector4 v = u * c;

    Quaternion q;
    q.MakeFromRotationMatrix(c);
    Matrix4x4 m;
    q.ToRotationMatrix(m);

    Vector4 w = u * m;

    BOOST_REQUIRE(w == v);
}

BOOST_AUTO_TEST_CASE(test_quaternion_make_from_angle_axis_x)
{
    Quaternion q;
    q.MakeFromAngleAxis(PI, Vector3::RIGHT);
    
    Matrix4x4 m;
    q.ToRotationMatrix(m);

    Vector4 v = Vector4::UP;
    v *= m;

    BOOST_REQUIRE(v == -Vector4::UP);
}

BOOST_AUTO_TEST_CASE(test_quaternion_make_from_angle_axis_y)
{
    Quaternion q;
    q.MakeFromAngleAxis(PI, Vector3::UP);
    
    Matrix4x4 m;
    q.ToRotationMatrix(m);

    Vector4 v = Vector4::LOOK;
    v *= m;

    BOOST_REQUIRE(v == -Vector4::LOOK);
}

BOOST_AUTO_TEST_CASE(test_quaternion_make_from_angle_axis_z)
{
    Quaternion q;
    q.MakeFromAngleAxis(PI, Vector3::LOOK);
    
    Matrix4x4 m;
    q.ToRotationMatrix(m);

    Vector4 v = Vector4::RIGHT;
    v *= m;

    BOOST_REQUIRE(v == -Vector4::RIGHT);
}

BOOST_AUTO_TEST_CASE(test_quaternion_make_from_axes)
{
    Quaternion q;
    q.MakeFromAxes(Vector3::RIGHT, Vector3::UP, Vector3::LOOK);

    Matrix3x3 m;
    q.ToRotationMatrix(m);

    Vector3 v = Vector3::ONE;
    v *= m;

    BOOST_REQUIRE(v == Vector3::ONE);

    q.MakeFromAxes(Vector3::RIGHT, Vector3::LOOK, -Vector3::UP);
    q.ToRotationMatrix(m);
    v *= m;

    BOOST_REQUIRE(v == Vector3(1.0f, -1.0f, 1.0f));
}

BOOST_AUTO_TEST_CASE(test_quaternion_to_angle_axis)
{
    Quaternion q;
    Matrix3x3 m, n;
    m.RotationAroundX(PI_OVER_2);
    m.RotationAroundZ(50 * DEG_2_RAD);
    q.MakeFromRotationMatrix(m);

    Vector3 u = Vector3::ONE;
    Vector3 v = u * m;

    float angle;
    Vector3 axis;
    q.ToAngleAxis(angle, axis);
    q.MakeFromAngleAxis(angle, axis);
    q.ToRotationMatrix(n);

    Vector3 w = u * n;

    BOOST_REQUIRE(v == w);
}

BOOST_AUTO_TEST_CASE(test_quaternion_to_axes)
{
    Quaternion q;
    Matrix3x3 m;
    m.RotationAroundX(PI_OVER_2);

    q.MakeFromRotationMatrix(m);
    Vector3 xAxis, yAxis, zAxis;

    xAxis = q.GetXAxis();
    yAxis = q.GetYAxis();
    zAxis = q.GetZAxis();

    BOOST_REQUIRE(xAxis ==  Vector3::RIGHT);
    BOOST_REQUIRE(yAxis == -Vector3::LOOK);
    BOOST_REQUIRE(zAxis ==  Vector3::UP);

    q.ToAxes(xAxis, yAxis, zAxis);

    BOOST_REQUIRE(xAxis ==  Vector3::RIGHT);
    BOOST_REQUIRE(yAxis == -Vector3::LOOK);
    BOOST_REQUIRE(zAxis ==  Vector3::UP);
}

BOOST_AUTO_TEST_CASE(test_quaternion_add_rotation)
{
    Quaternion q, r, t, k;
    q.MakeFromAngleAxis(-PI_OVER_2, Vector3::RIGHT);
    r.MakeFromAngleAxis(-40.0f * DEG_2_RAD, Vector3::UP);
    t.MakeFromAngleAxis(-310.0f * DEG_2_RAD, Vector3::LOOK);

    k = q * r * t;

    Vector4 v = Vector4::ONE;
    Matrix4x4 m;
    k.ToRotationMatrix(m);
    v = m * v;

    BOOST_REQUIRE(v == Vector4(-0.737208f, 0.686816f, -1.40883f));
}
