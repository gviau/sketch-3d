#include <boost/test/unit_test.hpp>

#include "SceneGraph.h"

#include "Matrix4x4.h"
#include "Node.h"
#include "Spatial.h"
#include "RenderState.h"
using namespace Sketch3D;

BOOST_AUTO_TEST_CASE(test_scene_graph_roots)
{
    SceneGraph graph;
    Node* spatialRoot = graph.GetNode("SpatialRoot");
    Node* renderRoot = graph.GetNode("RenderStateRoot");

    BOOST_REQUIRE(spatialRoot->GetName() == "SpatialRoot");
    BOOST_REQUIRE(renderRoot->GetName() == "RenderStateRoot");
    BOOST_REQUIRE(graph.GetRoot()->GetName() == "RenderStateRoot");
}

BOOST_AUTO_TEST_CASE(test_scene_graph_create_spatial_node)
{
    SceneGraph graph;

    Spatial* spatialNode = graph.CreateSpatialNode("spatial");

    BOOST_REQUIRE(spatialNode->GetTransformation() == Matrix4x4::IDENTITY);

    spatialNode->SetTransformation(Matrix4x4::ZERO);
    Spatial* node = graph.GetSpatialNode("spatial");

    BOOST_REQUIRE(node->GetTransformation() == Matrix4x4::ZERO);

    Spatial* invalid = graph.CreateSpatialNode("spatial");

    BOOST_REQUIRE(invalid == 0);
}

BOOST_AUTO_TEST_CASE(test_scene_graph_add_children)
{
    SceneGraph graph;

    Spatial* child1 = graph.CreateSpatialNode("child1");
    RenderState* child2 = graph.CreateRenderStateNode("child2");
    Spatial* child3 = graph.CreateSpatialNode("child3");

    Node* root = graph.GetNode("SpatialRoot");
    root->AddChild(child1);
    root->AddChild(child2);
    root->AddChild(child3);

    const list<Node*> children = root->GetChildren();
    BOOST_REQUIRE(children.size() == 3);
    BOOST_REQUIRE(child1->GetParent() == root);
    BOOST_REQUIRE(child2->GetParent() == root);
    BOOST_REQUIRE(child3->GetParent() == root);
}

BOOST_AUTO_TEST_CASE(test_scene_graph_delete_node)
{
    SceneGraph graph;

    Spatial* child1 = graph.CreateSpatialNode("child1");
    RenderState* child2 = graph.CreateRenderStateNode("child2");
    Spatial* child3 = graph.CreateSpatialNode("child3");

    Node* root = graph.GetNode("SpatialRoot");
    root->AddChild(child1);
    root->AddChild(child2);
    root->AddChild(child3);

    graph.DeleteNode("child2");

    BOOST_REQUIRE(root->GetChildren().size() == 2);

    graph.DeleteNode("RenderStateRoot");

    BOOST_REQUIRE(graph.GetNode("child1") == NULL);
    BOOST_REQUIRE(graph.GetNode("SpatialRooT") == NULL);
}
