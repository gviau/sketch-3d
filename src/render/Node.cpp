#include "Node.h"

namespace Sketch3D {

long long Node::nextNameIndex_ = 0;

Node::Node(Node* parent) : parent_(parent) {
	name_ = "NewNode" + std::to_string(nextNameIndex_);
	nextNameIndex_ += 1;
}

Node::Node(const string& name, Node* parent) : name_(name), parent_(parent) {
}

Node::Node(const Vector3& position, const Vector3& scale,
		   const Quaternion& orientation, Node* parent) : position_(position),
														  scale_(scale),
														  orientation_(orientation),
														  parent_(parent)
{
	name_ = "NewNode" + std::to_string(nextNameIndex_);
	nextNameIndex_ += 1;
}

Node::Node(const string& name, const Vector3& position, const Vector3& scale,
		   const Quaternion& orientation, Node* parent) : name_(name),
														  position_(position),
														  scale_(scale),
														  orientation_(orientation),
														  parent_(parent)
{
}

Node::~Node() {
}

void Node::render() const {
}

void Node::translate(const Vector3& translation) {
	position_ += translation;
}

void Node::scale(const Vector3& scale) {
	scale_.x *= scale.x;
	scale_.y *= scale.y;
	scale_.z *= scale.z;
}

void Node::pitch(float angle) {
	rotateAroundAxis(angle, Vector3::RIGHT);
}

void Node::yaw(float angle) {
	rotateAroundAxis(angle, Vector3::UP);
}

void Node::roll(float angle) {
	rotateAroundAxis(angle, Vector3::LOOK);
}

void Node::rotateAroundAxis(float angle, const Vector3& axis) {
	Quaternion rot;
	rot.MakeFromAngleAxis(angle, axis);
	orientation_ *= rot;
}

void Node::setParent(Node* parent) {
	parent_ = parent;
}

void Node::setPosition(const Vector3& position) {
	position_ = position;
}

void Node::setScale(const Vector3& scale) {
	scale_ = scale;
}

void Node::setOrientation(const Quaternion& orientation) {
	orientation_ = orientation;
}

const string& Node::getName() const {
	return name_;
}

Node* Node::getParent() const {
	return parent_;
}

const Vector3& Node::getPosition() const {
	return position_;
}

const Vector3& Node::getScale() const {
	return scale_;
}

const Quaternion& Node::getOrientation() const {
	return orientation_;
}

}