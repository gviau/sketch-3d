#include "Node.h"

#include <algorithm>
#include <queue>
#include <iostream>

#include "Renderer.h"
#include "SceneGraph.h"

namespace Sketch3D
{
Node::Node(const string& name) : parent_(0), name_(name)
{
}

Node::~Node()
{
}

void Node::PropagateRenderStateFromRoot(const RenderState& newState)
{
    // Start the propagation from root
    queue<Node*> toPropagate;
    toPropagate.push(Renderer::GetInstance()->GetSceneGraph().GetRoot());

    while (!toPropagate.empty()) {
        Node* current = toPropagate.front();
        toPropagate.pop();

        current->UpdateRenderState(newState);
    
        list<Node*>::iterator it = current->children_.begin();
        for (; it != current->children_.end(); ++it) {
            toPropagate.push((*it));
        }
    }
}

void Node::UpdateRenderState(const RenderState& newState)
{
    // Should be implemented in derived class
}

void Node::AddChild(Node* node)
{
    // If the node was already attached to a node, we have to restructure the
    // graph.
    if (node->parent_ != 0) {
        node->parent_->RemoveChild(node);
    }

    node->parent_ = this;
    children_.push_back(node);
}

void Node::RemoveChild(Node* node)
{
    list<Node*>::iterator new_end;
    new_end = remove(children_.begin(), children_.end(), node);
    children_.erase(new_end, children_.end());
}

void Node::RemoveChildByIndex(size_t index)
{
    if (index >= children_.size()) {
        return;
    }

    list<Node*>::iterator it = children_.begin();
    for (size_t i = 0; i < index; i++) {
        ++it;
    }

    delete (*it);
    children_.erase(it);
}

void Node::RemoveChildByName(const string& name)
{
    list<Node*>::iterator new_end;
    new_end = remove_if(children_.begin(), children_.end(), HasSameName(name));
    children_.erase(new_end, children_.end());
}

const list<Node*>& Node::GetChildren() const
{
    return children_;
}

list<Node*>& Node::GetChildren()
{
    return children_;
}

Node* Node::GetChildByIndex(size_t index) const
{
    if (index >= children_.size()) {
        return 0;
    }

    list<Node*>::const_iterator it = children_.begin();
    for (size_t i = 0; i < index; i++) {
        ++it;
    }

    return (*it);
}

Node* Node::GetChildByName(const string& name) const
{
    list<Node*>::const_iterator result;
    result = find_if(children_.begin(), children_.end(), HasSameName(name));

    if (result == children_.end()) {
        return 0;
    }

    return *result;
}

bool Node::ReplaceNodeByIndex(size_t index, Node* node)
{
    if (index >= children_.size()) {
        return false;
    }

    list<Node*>::iterator it = children_.begin();
    for (size_t i = 0; i < index; i++) {
        ++it;
    }

    delete (*it);
    node->parent_ = this;
    children_.insert(it, node);

    return true;
}

bool Node::ReplaceNodeByName(const string& name, Node* node)
{
    list<Node*>::iterator it;
    it = find_if(children_.begin(), children_.end(), HasSameName(name));

    if (it == children_.end()) {
        return false;
    }

    delete (*it);
    node->parent_ = this;
    children_.insert(it, node);
    
    return true;
}

void Node::RemoveAllChildren()
{
    children_.clear();
}

void Node::SetParent(Node* parent)
{
    parent_ = parent;
}

Node* Node::GetParent() const
{
    return parent_;
}

const string& Node::GetName() const
{
    return name_;
}
}
