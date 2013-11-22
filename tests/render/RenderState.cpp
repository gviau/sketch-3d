#include <boost/test/unit_test.hpp>

#include "Geometry.h"
#include "GlobalState.h"
#include "Renderer.h"
#include "RenderState.h"
#include "SceneGraph.h"
#include "Spatial.h"
using namespace Sketch3D;

// Fixture. We just have to clear the graph after each test
struct RenderStateFixture {
    RenderStateFixture() : graph(Renderer::GetInstance()->GetSceneGraph()) {}
    ~RenderStateFixture() { graph.ClearKeepRoots(); }

    SceneGraph& graph;
};

BOOST_FIXTURE_TEST_SUITE(render_suite, RenderStateFixture);

BOOST_AUTO_TEST_CASE(test_renderstate_update_render_state)
{
    RenderState* root = graph.GetRenderStateNode("RenderStateRoot");

    RenderState newRenderState("dummy");
    newRenderState.AddGlobalState(new ZBufferState());
    newRenderState.AddGlobalState(new FogState());
    newRenderState.AddGlobalState(new CullState());
    newRenderState.RemoveGlobalState(GlobalState::FOG);

    root->UpdateRenderState(newRenderState);
    
    BOOST_REQUIRE(root->GetGlobalState(GlobalState::ALPHA) == NULL);
    BOOST_REQUIRE(root->GetGlobalState(GlobalState::CULL) != NULL);
    BOOST_REQUIRE(root->GetGlobalState(GlobalState::FOG) == NULL);
    BOOST_REQUIRE(root->GetGlobalState(GlobalState::MATERIAL) == NULL);
    BOOST_REQUIRE(root->GetGlobalState(GlobalState::WIREFRAME) == NULL);
    BOOST_REQUIRE(root->GetGlobalState(GlobalState::ZBUFFER) != NULL);
}

BOOST_AUTO_TEST_CASE(test_renderstate_propagate_render_state_from_root)
{
    RenderState* root = graph.GetRenderStateNode("RenderStateRoot");
    Spatial* child1 = graph.CreateSpatialNode("child1");
    Geometry* child1_1 = graph.CreateGeometryNode("child1_1");
    Geometry* child1_2 = graph.CreateGeometryNode("child1_2");
    RenderState* test1 = graph.CreateRenderStateNode("test1");
    Spatial* child1_3_1 = graph.CreateSpatialNode("child1_3_1");
    RenderState* test2 = graph.CreateRenderStateNode("test2");

    child1->AddChild(child1_1);
    child1->AddChild(child1_2);
    child1->AddChild(test1);
    test1->AddChild(child1_3_1);
    child1_3_1->AddChild(test2);

    RenderState newRenderState("dummy");
    newRenderState.AddGlobalState(new ZBufferState());
    newRenderState.AddGlobalState(new FogState());
    newRenderState.AddGlobalState(new CullState());
    newRenderState.RemoveGlobalState(GlobalState::FOG);

    test2->PropagateRenderStateFromRoot(newRenderState);

    BOOST_REQUIRE(root->GetGlobalState(GlobalState::ALPHA) == NULL);
    BOOST_REQUIRE(root->GetGlobalState(GlobalState::CULL) != NULL);
    BOOST_REQUIRE(root->GetGlobalState(GlobalState::FOG) == NULL);
    BOOST_REQUIRE(root->GetGlobalState(GlobalState::MATERIAL) == NULL);
    BOOST_REQUIRE(root->GetGlobalState(GlobalState::WIREFRAME) == NULL);
    BOOST_REQUIRE(root->GetGlobalState(GlobalState::ZBUFFER) != NULL);

    BOOST_REQUIRE(test1->GetGlobalState(GlobalState::ALPHA) == NULL);
    BOOST_REQUIRE(test1->GetGlobalState(GlobalState::CULL) != NULL);
    BOOST_REQUIRE(test1->GetGlobalState(GlobalState::FOG) == NULL);
    BOOST_REQUIRE(test1->GetGlobalState(GlobalState::MATERIAL) == NULL);
    BOOST_REQUIRE(test1->GetGlobalState(GlobalState::WIREFRAME) == NULL);
    BOOST_REQUIRE(test1->GetGlobalState(GlobalState::ZBUFFER) != NULL);

    BOOST_REQUIRE(test2->GetGlobalState(GlobalState::ALPHA) == NULL);
    BOOST_REQUIRE(test2->GetGlobalState(GlobalState::CULL) != NULL);
    BOOST_REQUIRE(test2->GetGlobalState(GlobalState::FOG) == NULL);
    BOOST_REQUIRE(test2->GetGlobalState(GlobalState::MATERIAL) == NULL);
    BOOST_REQUIRE(test2->GetGlobalState(GlobalState::WIREFRAME) == NULL);
    BOOST_REQUIRE(test2->GetGlobalState(GlobalState::ZBUFFER) != NULL);
}

BOOST_AUTO_TEST_SUITE_END();
