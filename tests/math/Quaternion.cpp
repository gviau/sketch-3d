#include <boost/test/unit_test.hpp>

#include "Constants.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "Vector4.h"

using namespace Sketch3D;

#include <iostream>
using namespace std;

BOOST_AUTO_TEST_CASE(test_quaternion_construct_rotation)
{
    Quaternion q;
    Matrix4x4 m;
    q.ConstructRotationMatrix(m);

    BOOST_REQUIRE(m == Matrix4x4::IDENTITY);
}

BOOST_AUTO_TEST_CASE(test_quaternion_make_from_angle_axis_x)
{
    Quaternion q;
    q.MakeFromAngleAxis(PI, Vector3::RIGHT);
    
    Matrix4x4 m;
    q.ConstructRotationMatrix(m);

    Vector4 v = Vector4::UP;
    v *= m;

    BOOST_REQUIRE(v == -Vector4::UP);
}

BOOST_AUTO_TEST_CASE(test_quaternion_make_from_angle_axis_y)
{
    Quaternion q;
    q.MakeFromAngleAxis(PI, Vector3::UP);
    
    Matrix4x4 m;
    q.ConstructRotationMatrix(m);

    Vector4 v = Vector4::LOOK;
    v *= m;

    BOOST_REQUIRE(v == -Vector4::LOOK);
}

BOOST_AUTO_TEST_CASE(test_quaternion_make_from_angle_axis_z)
{
    Quaternion q;
    q.MakeFromAngleAxis(PI, Vector3::LOOK);
    
    Matrix4x4 m;
    q.ConstructRotationMatrix(m);

    Vector4 v = Vector4::RIGHT;
    v *= m;

    BOOST_REQUIRE(v == -Vector4::RIGHT);
}

BOOST_AUTO_TEST_CASE(test_quaternion_make_from_euler)
{
    Quaternion q;
    q.MakeFromAngleAxis(PI, Vector3::RIGHT);
    
    Matrix4x4 m;
    q.ConstructRotationMatrix(m);

    Matrix4x4 r;
    r.RotationAroundX(PI);

    cout << endl;
    BOOST_REQUIRE(m == r);

    Vector4 v = Vector4::UP;
    v *= m;

    BOOST_REQUIRE(v == -Vector4::UP);

    Quaternion a, b, c;
    a.MakeFromAngleAxis(PI_OVER_2, Vector3::RIGHT);
    b.MakeFromAngleAxis(40 * DEG_2_RAD, Vector3::UP);
    c.MakeFromAngleAxis(310 * DEG_2_RAD, Vector3::LOOK);

    q = c * b * a;
    q.Normalize();
    q.ConstructRotationMatrix(m);


    v = Vector4::ONE;
    v = m * v;

    BOOST_REQUIRE(v == Vector4(-0.737208f, 0.686816f, -1.40883f));
}

BOOST_AUTO_TEST_CASE(test_quaternion_add_rotation)
{
    Quaternion q, r, t, k;
    q.MakeFromAngleAxis(PI_OVER_2, Vector3::RIGHT);
    r.MakeFromAngleAxis(40.0f * DEG_2_RAD, Vector3::UP);
    t.MakeFromAngleAxis(310.0f * DEG_2_RAD, Vector3::LOOK);

    k = t * r * q;
    k.Normalize();

    Vector4 v = Vector4::ONE;
    Matrix4x4 m;
    k.ConstructRotationMatrix(m);

    v *= m;

    BOOST_REQUIRE(v == Vector4(-0.737208f, 0.686816f, -1.40883f));
}
