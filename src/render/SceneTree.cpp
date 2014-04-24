#include "render/SceneTree.h"

namespace Sketch3D {

SceneTree::SceneTree() : CompositeNode("root") {
}

SceneTree::~SceneTree() {
}

void SceneTree::Render() const {
	CompositeNode::Render();
}

}