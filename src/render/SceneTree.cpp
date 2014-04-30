#include "render/SceneTree.h"

namespace Sketch3D {

SceneTree::SceneTree() : CompositeNode("root") {
}

SceneTree::~SceneTree() {
}

void SceneTree::Render() const {
	map<string, Node*>::const_iterator it = children_.begin();
	for (; it != children_.end(); ++it) {
		it->second->Render();
	}
}

}