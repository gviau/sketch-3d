#include "render/CompositeNode.h"

namespace Sketch3D {

CompositeNode::CompositeNode(Node* parent) : Node(parent) {
}

CompositeNode::CompositeNode(const string& name, Node* parent) : Node(name, parent) {
}

CompositeNode::CompositeNode(const Vector3& position, const Vector3& scale,
							 const Quaternion& orientation, Node* parent) : Node(position,
																				 scale,
																				 orientation,
																				 parent)
{
}

CompositeNode::CompositeNode(const string& name, const Vector3& position,
							 const Vector3& scale, const Quaternion& orientation,
							 Node* parent) : Node(name, position, scale, orientation,
												  parent)
{
}

CompositeNode::~CompositeNode() {
}

void CompositeNode::Render() const {
	map<string, Node>::const_iterator it = children_.begin();
	for (; it != children_.end(); ++it) {
		it->second.Render();
	}
}

bool CompositeNode::AddChildren(const Node& node) {
	string name = node.GetName();
	map<string, Node>::iterator it = children_.find(name);
	if (it != children_.end()) {
		return false;
	}

	children_[name] = node;
	return true;
}

bool CompositeNode::RemoveChildren(const string& name) {
	map<string, Node>::iterator it = children_.find(name);
	if (it == children_.end()) {
		return false;
	}

	children_.erase(it);
	return true;
}

}