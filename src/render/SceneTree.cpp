#include "render/SceneTree.h"

#include "render/Material.h"
#include "render/Mesh.h"
#include "render/Node.h"
#include "render/RenderQueue.h"

namespace Sketch3D {

SceneTree::SceneTree() {
}

SceneTree::~SceneTree() {
}

void SceneTree::Render() {
    map<string, Node*>::iterator it = root_.children_.begin();
    for (; it != root_.children_.end(); ++it) {
        it->second->Render(renderQueue_);
    }

    renderQueue_.Render();
}

bool SceneTree::AddNode(Node* node) {
    return root_.AddChildren(node);
}

Node* SceneTree::GetNodeByName(const string& name) const {
    return root_.GetNodeByName(name);
}

bool SceneTree::RemoveNode(const Node* const node) {
    return root_.RemoveChildren(node);
}

bool SceneTree::RemoveNodeByName(const string& name) {
    return root_.RemoveChildrenByName(name);
}

}