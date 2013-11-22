#include "SceneGraph.h"

#include <queue>

#include "Geometry.h"
#include "RenderState.h"
#include "RenderSystem.h"
#include "Spatial.h"

namespace Sketch3D
{
SceneGraph::SceneGraph()
{
    string renderStateName = "RenderStateRoot";
    RenderState* state = new RenderState(renderStateName);
    root_ = state;
    nodes_[renderStateName] = state;

    string spatialName = "SpatialRoot";
    Spatial* spatial = new Spatial(spatialName);
    root_->AddChild(spatial);
    nodes_[spatialName] = spatial;
    spatialRoot_ = spatial;
}

SceneGraph::~SceneGraph()
{
    Clear();
}

void SceneGraph::Draw(RenderSystem* renderSystem) const
{
    queue<Node*> nodes;
    nodes.push(root_);

    while (!nodes.empty()) {
        Node* current = nodes.front();
        nodes.pop();

        current->Draw(renderSystem);

        list<Node*> children = current->GetChildren();
        list<Node*>::iterator it = children.begin();
        for (; it != children.end(); ++it) {
            nodes.push(*it);
        }
    }
}

RenderState* SceneGraph::CreateRenderStateNode(const string& name)
{
    if (nodes_.find(name) != nodes_.end()) {
        return NULL;
    }

    RenderState* newNode = new RenderState(name);
    spatialRoot_->AddChild(newNode);
    nodes_[name] = newNode;

    return newNode;
}

Spatial* SceneGraph::CreateSpatialNode(const string& name)
{
    if (nodes_.find(name) != nodes_.end()) {
        return NULL;
    }

    Spatial* newNode = new Spatial(name);
    spatialRoot_->AddChild(newNode);
    nodes_[name] = newNode;

    // Do we need a new spatial root?
    if (spatialRoot_ == 0) {
        spatialRoot_ = newNode;
    }

    return newNode;
}

Geometry* SceneGraph::CreateGeometryNode(const string& name)
{
    if (nodes_.find(name) != nodes_.end()) {
        return NULL;
    }

    Geometry* newNode = new Geometry(name);
    spatialRoot_->AddChild(newNode);
    nodes_[name] = newNode;

    return newNode;
}

void SceneGraph::DeleteNode(const string& name)
{
    map<string, Node*>::iterator it = nodes_.find(name);
    if (it == nodes_.end()) {
        return;
    }

    // Remove all children
    vector<Node*> toDelete;
    queue<Node*> nodes;
    nodes.push(it->second);

    while (!nodes.empty()) {
        Node* node = nodes.front();
        nodes.pop();

        list<Node*> children = node->GetChildren();
        list<Node*>::iterator c_it = children.begin();
        for (; c_it != children.end(); ++c_it) {
            nodes.push(*c_it);
        }

        // Remove it from the scene graph
        it = nodes_.find(node->GetName());
        nodes_.erase(it);

        // Remove if from its parent's children list as well if it has one
        Node* parent = node->GetParent();
        if (parent != 0) {
            parent->RemoveChildByName(node->GetName());
        }

        toDelete.push_back(node);
    }

    // Free memory
    for (size_t i = 0; i < toDelete.size(); i++) {
        delete toDelete[i];
    }
}

Node* SceneGraph::GetRoot() const
{
    return root_;
}

Spatial* SceneGraph::GetSpatialRoot() const
{
    return spatialRoot_;
}

Node* SceneGraph::GetNode(const string& name) const
{
    map<string, Node*>::const_iterator it = nodes_.find(name);
    if (it == nodes_.end()) {
        return NULL;
    }

    return it->second;
}

Spatial* SceneGraph::GetSpatialNode(const string& name) const
{
    return static_cast<Spatial*>(GetNode(name));
}

RenderState* SceneGraph::GetRenderStateNode(const string& name) const
{
    return static_cast<RenderState*>(GetNode(name));
}

Geometry* SceneGraph::GetGeometryNode(const string& name) const
{
    return static_cast<Geometry*>(GetNode(name));
}

void SceneGraph::Clear()
{
    map<string, Node*>::iterator it;
    for (it = nodes_.begin(); it != nodes_.end(); ++it) {
        delete it->second;
        it->second = NULL;
    }

    root_ = 0;
    spatialRoot_ = 0;
}

void SceneGraph::ClearKeepRoots()
{
    map<string, Node*>::iterator it;
    for (it = nodes_.begin(); it != nodes_.end(); ++it) {
        Node* node = it->second;
        if (node == root_ || node == spatialRoot_) {
            continue;
        }

        delete node;
        nodes_.erase(it);
    }

    root_->RemoveAllChildren();
    spatialRoot_->RemoveAllChildren();
    root_->AddChild(spatialRoot_);
    spatialRoot_->SetTransformation(Matrix4x4::IDENTITY);
}
}
