#include <boost/test/unit_test.hpp>

#include "Geometry.h"
#include "Matrix4x4.h"
#include "Renderer.h"
#include "SceneGraph.h"
#include "Spatial.h"
#include "Vector3.h"
using namespace Sketch3D;

// Fixture. We just have to clear the graph after each test
struct GeometryFixture {
    GeometryFixture() : graph(Renderer::GetInstance()->GetSceneGraph()) {}
    ~GeometryFixture() { graph.ClearKeepRoots(); }

    SceneGraph& graph;
};

BOOST_FIXTURE_TEST_SUITE(render_suite, GeometryFixture);

BOOST_AUTO_TEST_CASE(test_geometry_translate)
{
    Geometry* geometry = graph.CreateGeometryNode("test");
    geometry->Translate(10.0f, 0.0f, 0.0f);

    Spatial* spatial = graph.GetSpatialNode(geometry->GetParent()->GetName());
    Matrix4x4 mat;
    mat = spatial->GetTransformation();

    Matrix4x4 result( 1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                     10.0f, 0.0f, 0.0f, 1.0f);

    BOOST_REQUIRE(mat == result);

    geometry->Translate(Vector3(5.0f, 10.0f, 10.0f));
    mat = spatial->GetTransformation();
    result(0, 3) = 15.0f;
    result(1, 3) = 10.0f;
    result(2, 3) = 10.0f;

    BOOST_REQUIRE(mat == result);
}

BOOST_AUTO_TEST_CASE(test_geometry_translate_many_children)
{
    Geometry* dummy = graph.CreateGeometryNode("dummy");
    Geometry* geometry = graph.CreateGeometryNode("test");
    dummy->AddChild(geometry);
    geometry->Translate(10.0f, 0.0f, 0.0f);

    Spatial* root = graph.GetSpatialRoot();

    Matrix4x4 mat = root->GetTransformation();

    BOOST_REQUIRE(mat == Matrix4x4::IDENTITY);

    Matrix4x4 result( 1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                     10.0f, 0.0f, 0.0f, 1.0f);

    Spatial* spatial = graph.GetSpatialNode(geometry->GetParent()->GetName());
    mat = spatial->GetTransformation();

    BOOST_REQUIRE(mat == result);
}

BOOST_AUTO_TEST_CASE(test_geometry_roation)
{
    Geometry* dummy = graph.CreateGeometryNode("dummy");
    Geometry* geometry = graph.CreateGeometryNode("test");
    dummy->AddChild(geometry);
    geometry->RotateX(PI_OVER_2);

    Spatial* root = graph.GetSpatialRoot();
    Matrix4x4 mat = root->GetTransformation();

    BOOST_REQUIRE(mat == Matrix4x4::IDENTITY);

    Matrix4x4 result(1.0f,  0.0f, 0.0f, 0.0f,
                     0.0f,  0.0f, 1.0f, 0.0f,
                     0.0f, -1.0f, 0.0f, 0.0f,
                     0.0f,  0.0f, 0.0f, 1.0f);

    Spatial* spatial = graph.GetSpatialNode(geometry->GetParent()->GetName());
    mat = spatial->GetTransformation();

    BOOST_REQUIRE(mat == result);

    geometry->RotateZ(50 * DEG_2_RAD);
    geometry->RotateY(110 * DEG_2_RAD);

    mat = spatial->GetTransformation();
    result(0, 0) = -0.2198463f;
    result(1, 0) =  0.7660444f;
    result(2, 0) = -0.6040227f;

    result(0, 1) =  0.9396926f;
    result(1, 1) =  0.0f;
    result(2, 1) = -0.3420201f;

    result(0, 2) = -0.2620026f;
    result(1, 2) = -0.6427886f;
    result(2, 2) = -0.7198463f;

    BOOST_REQUIRE(mat == result);
}

BOOST_AUTO_TEST_CASE(test_geometry_scale)
{
    Geometry* dummy = graph.CreateGeometryNode("dummy");
    Geometry* geometry = graph.CreateGeometryNode("test");
    dummy->AddChild(geometry);
    geometry->Scale(2.0f, 1.0f, 1.0f);

    Spatial* root = graph.GetSpatialRoot();
    Matrix4x4 mat = root->GetTransformation();

    BOOST_REQUIRE(mat == Matrix4x4::IDENTITY);

    Matrix4x4 result = Matrix4x4(2.0f, 0.0f, 0.0f, 0.0f,
                                 0.0f, 1.0f, 0.0f, 0.0f,
                                 0.0f, 0.0f, 1.0f, 0.0f,
                                 0.0f, 0.0f, 0.0f, 1.0f);

    Spatial* spatial = graph.GetSpatialNode(geometry->GetParent()->GetName());
    mat = spatial->GetTransformation();

    BOOST_REQUIRE(mat == result);

    geometry->Scale(Vector3(4.0f, 2.0f, 2.0f));
    result(0, 0) = 8.0f;
    result(1, 1) = 2.0f;
    result(2, 2) = 2.0f;
    mat = spatial->GetTransformation();

    BOOST_REQUIRE(mat == result);
}

BOOST_AUTO_TEST_CASE(test_geometry_combine_transform)
{
    Geometry* geometry = graph.CreateGeometryNode("test");
    geometry->Scale(2.0f, 2.0f, 2.0f);
    geometry->RotateY(PI_OVER_2);
    geometry->Translate(5.0f, 0.0f, 3.0f);

    Spatial* root = graph.GetSpatialRoot();
    Matrix4x4 mat = root->GetTransformation();

    Matrix4x4 result(0.0f, 0.0f, -2.0f, 0.0f,
                     0.0f, 2.0f,  0.0f, 0.0f,
                     2.0f, 0.0f,  0.0f, 0.0f,
                     5.0f, 0.0f,  3.0f, 1.0f);

    BOOST_REQUIRE(mat == result);
}

BOOST_AUTO_TEST_SUITE_END();
