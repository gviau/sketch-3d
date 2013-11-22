#include <boost/test/unit_test.hpp>

#include "Spatial.h"
using namespace Sketch3D;

BOOST_AUTO_TEST_CASE(test_node_get_child_by_index)
{
    Spatial* spatial = new Spatial("test");
    Spatial* dummy = new Spatial("dummy");
    Spatial* test = new Spatial("child");
    spatial->AddChild(dummy);
    spatial->AddChild(test);

    BOOST_REQUIRE(spatial->GetChildByIndex(1)->GetName() == "child");
}

BOOST_AUTO_TEST_CASE(test_node_get_child_by_name)
{
    Spatial* spatial = new Spatial("test");
    Spatial* dummy = new Spatial("dummy");
    Spatial* test = new Spatial("child");
    spatial->AddChild(dummy);
    spatial->AddChild(test);

    BOOST_REQUIRE(spatial->GetChildByName("dummy")->GetName() == "dummy");
}

BOOST_AUTO_TEST_CASE(test_node_remove_child)
{
    Spatial* spatial = new Spatial("test");
    Spatial* dummy = new Spatial("dummy");
    Spatial* test = new Spatial("child");
    spatial->AddChild(dummy);
    spatial->AddChild(test);
    spatial->RemoveChild(dummy);

    BOOST_REQUIRE(spatial->GetChildren().size() == 1);
}

BOOST_AUTO_TEST_CASE(test_node_remove_child_by_index)
{
    Spatial* spatial = new Spatial("test");
    Spatial* dummy = new Spatial("dummy");
    Spatial* test = new Spatial("child");
    spatial->AddChild(dummy);
    spatial->AddChild(test);
    spatial->AddChild(new Spatial("child2"));
    spatial->RemoveChildByIndex(1);

    BOOST_REQUIRE(spatial->GetChildren().size() == 2);
    BOOST_REQUIRE(spatial->GetChildByIndex(1)->GetName() == "child2");
}

BOOST_AUTO_TEST_CASE(test_node_remove_child_by_name)
{
    Spatial* spatial = new Spatial("test");
    Spatial* dummy = new Spatial("dummy");
    Spatial* test = new Spatial("child");
    spatial->AddChild(dummy);
    spatial->AddChild(test);
    spatial->AddChild(new Spatial("child2"));
    spatial->RemoveChildByName("child");

    BOOST_REQUIRE(spatial->GetChildren().size() == 2);
    BOOST_REQUIRE(spatial->GetChildByIndex(1)->GetName() == "child2");
}

BOOST_AUTO_TEST_CASE(test_node_remove_all_children)
{
    Spatial* spatial = new Spatial("test");
    Spatial* dummy = new Spatial("dummy");
    Spatial* test = new Spatial("child");
    spatial->AddChild(dummy);
    spatial->AddChild(test);
    spatial->RemoveAllChildren();

    BOOST_REQUIRE(spatial->GetChildren().size() == 0);
}

BOOST_AUTO_TEST_CASE(test_node_replace_node_by_index)
{
    Spatial* spatial = new Spatial("test");
    Spatial* dummy = new Spatial("dummy");
    Spatial* test = new Spatial("child");
    spatial->AddChild(dummy);
    spatial->AddChild(test);
    spatial->AddChild(new Spatial("child2"));

    BOOST_REQUIRE(spatial->GetChildByIndex(1)->GetName() == "child");
    spatial->ReplaceNodeByIndex(1, new Spatial("child3"));
    BOOST_REQUIRE(spatial->GetChildByIndex(1)->GetName() == "child3");
}

BOOST_AUTO_TEST_CASE(test_node_replace_node_by_name)
{
    Spatial* spatial = new Spatial("test");
    Spatial* dummy = new Spatial("dummy");
    Spatial* test = new Spatial("child");
    spatial->AddChild(dummy);
    spatial->AddChild(test);
    spatial->AddChild(new Spatial("child2"));

    BOOST_REQUIRE(spatial->GetChildByIndex(1)->GetName() == "child");
    spatial->ReplaceNodeByName("child", new Spatial("child3"));
    BOOST_REQUIRE(spatial->GetChildByIndex(1)->GetName() == "child3");
}
