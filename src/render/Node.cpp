#include "render/Node.h"

#include "render/BufferObject.h"
#include "render/Material.h"
#include "render/Mesh.h"
#include "render/ModelManager.h"
#include "render/Renderer.h"
#include "render/RenderQueue.h"
#include "render/Shader.h"
#include "render/SkinnedMesh.h"
#include "render/Texture2D.h"

#include <sstream>

namespace Sketch3D {

long long Node::nextNameIndex_ = 0;

Node::Node(Node* parent) : parent_(parent), mesh_(NULL), material_(NULL),
						   scale_(1.0f, 1.0f, 1.0), needTransformationUpdate_(true)
{
    ostringstream convert;
    convert << nextNameIndex_;
	name_ = "NewNode" + convert.str();
	nextNameIndex_ += 1;
}

Node::Node(const string& name, Node* parent) : name_(name), parent_(parent),
											   mesh_(NULL), material_(NULL),
											   scale_(1.0f, 1.0f, 1.0f), needTransformationUpdate_(true)
{
}

Node::Node(const Vector3& position, const Vector3& scale,
		   const Quaternion& orientation, Node* parent) : parent_(parent),
                                                          position_(position),
														  scale_(scale),
														  orientation_(orientation),
														  mesh_(NULL),
														  material_(NULL),
                                                          needTransformationUpdate_(true)
{
    ostringstream convert;
    convert << nextNameIndex_;
	name_ = "NewNode" + convert.str();
	nextNameIndex_ += 1;
}

Node::Node(const string& name, const Vector3& position, const Vector3& scale,
		   const Quaternion& orientation, Node* parent) : name_(name),
                                                          parent_(parent),
														  position_(position),
														  scale_(scale),
														  orientation_(orientation),
														  mesh_(NULL),
														  material_(NULL),
                                                          needTransformationUpdate_(true)
{
}

Node::Node(const Node& src) : parent_(src.parent_),
                              position_(src.position_),
                              scale_(src.scale_),
                              orientation_(src.orientation_),
                              material_(src.material_),
                              needTransformationUpdate_(true)
{
    // TODO
    // Better manage name copy
    name_ = src.name_ + "_c";

    // TODO
    // Better manager mesh and textures copy
    *mesh_  = *src.mesh_;
}

Node::~Node() {
}

void Node::Render() {
	Shader* shader = material_->GetShader();

    const Matrix4x4& viewProjection = Renderer::GetInstance()->GetViewProjectionMatrix();
    const Matrix4x4& view = Renderer::GetInstance()->GetViewMatrix();

    // Setup the transformation matrix for this node
    const Matrix4x4& model = ConstructModelMatrix();
    Matrix4x4 modelViewProjection(viewProjection * model);
    Matrix4x4 modelView(view * model);

    // Set the uniform matrices
    Renderer::GetInstance()->BindShader(shader);
    shader->SetUniformMatrix4x4("modelViewProjection", modelViewProjection);
    shader->SetUniformMatrix4x4("modelView", modelView);
    shader->SetUniformMatrix4x4("model", model);
    shader->SetUniformMatrix4x4("view", Renderer::GetInstance()->GetViewMatrix());

    // Get the rendering data
    BufferObject** bufferObjects;
    vector<ModelSurface_t> surfaces;
    mesh_->GetRenderInfo(bufferObjects, surfaces);

    // Material's textures
    const map<string, Texture*>& materialTextures = material_->GetTextures();
    map<string, Texture*>::const_iterator it = materialTextures.begin();

    for (; it != materialTextures.end(); ++it) {
        if (it->second != nullptr) {
            shader->SetUniformTexture(it->first, it->second);
        }
    }

    // Render the mesh
    for (size_t i = 0; i < surfaces.size(); i++) {
        // Textures associated with the surface
        for (size_t j = 0; j < surfaces[i].geometry->numTextures; j++) {
            Texture2D* texture = surfaces[i].geometry->textures[j];
            if (texture != nullptr) {
                shader->SetUniformTexture("texture" + to_string(j), texture);
            }
        }

        bufferObjects[i]->Render();
    }
}

bool Node::AddChildren(Node* node) {
	string name = node->GetName();
	map<string, Node*>::iterator it = children_.find(name);
	if (it != children_.end()) {
		return false;
	}

	children_[name] = node;
	return true;
}

Node* Node::GetNodeByName(const string& name) const {
    map<string, Node*>::const_iterator it = children_.find(name);
    if (it != children_.end()) {
        return it->second;
    }

    Node* node = nullptr;
    for (it = children_.begin(); it != children_.end(); ++it) {
        node = it->second->GetNodeByName(name);

        if (node != nullptr) {
            return node;
        }
    }

    return nullptr;
}

bool Node::RemoveChildrenByName(const string& name) {
	map<string, Node*>::iterator it = children_.find(name);
	if (it != children_.end()) {
	    children_.erase(it);
	    return true;
	}

    for (it = children_.begin(); it != children_.end(); ++it) {
        if (it->second->RemoveChildrenByName(name)) {
            return true;
        }
    }

    return false;
}

bool Node::RemoveChildren(const Node* const node) {
    return RemoveChildrenByName(node->GetName());
}

void Node::Translate(const Vector3& translation) {
	position_ += translation;
    needTransformationUpdate_ = true;
}

void Node::Scale(const Vector3& scale) {
	scale_.x *= scale.x;
	scale_.y *= scale.y;
	scale_.z *= scale.z;
    needTransformationUpdate_ = true;
}

void Node::Pitch(float angle) {
	RotateAroundAxis(angle, Vector3::RIGHT);
}

void Node::Yaw(float angle) {
	RotateAroundAxis(angle, Vector3::UP);
}

void Node::Roll(float angle) {
	RotateAroundAxis(angle, Vector3::LOOK);
}

void Node::RotateAroundAxis(float angle, const Vector3& axis) {
	Quaternion rot;
	rot.MakeFromAngleAxis(angle, axis);
	rot.Normalize();
	orientation_ = rot * orientation_;
    needTransformationUpdate_ = true;
}

Matrix4x4 Node::ConstructModelMatrix() {
    if (needTransformationUpdate_) {
        Matrix4x4 model;
        model[0][0] = scale_.x;
        model[1][1] = scale_.y;
        model[2][2] = scale_.z;

        Matrix4x4 rotation;
        orientation_.ToRotationMatrix(rotation);
        model = rotation * model;

        model[0][3] = position_.x;
        model[1][3] = position_.y;
        model[2][3] = position_.z;

        needTransformationUpdate_ = false;
        cachedTransformation_ = model;
    }

    return cachedTransformation_;
}

void Node::SetParent(Node* parent) {
	parent_ = parent;
}

void Node::SetPosition(const Vector3& position) {
	position_ = position;
    needTransformationUpdate_ = true;
}

void Node::SetScale(const Vector3& scale) {
	scale_ = scale;
    needTransformationUpdate_ = true;
}

void Node::SetOrientation(const Quaternion& orientation) {
	orientation_ = orientation;
    needTransformationUpdate_ = true;
}

void Node::SetMesh(Mesh* mesh) {
	mesh_ = mesh;
}

void Node::SetMaterial(Material* material) {
	material_ = material;
}

const string& Node::GetName() const {
	return name_;
}

Node* Node::GetParent() const {
	return parent_;
}

const Vector3& Node::GetPosition() const {
	return position_;
}

const Vector3& Node::GetScale() const {
	return scale_;
}

const Quaternion& Node::GetOrientation() const {
	return orientation_;
}

Mesh* Node::GetMesh() const {
	return mesh_;
}

Material* Node::GetMaterial() const {
	return material_;
}

void Node::Render(const FrustumPlanes_t& frustumPlanes, bool useFrustumCulling, RenderQueue& renderQueue) {
    if (mesh_ != nullptr) {
        bool addMeshToRenderQueue = true;

        if (useFrustumCulling) {
            float maxScaleValue = max(scale_.x, max(scale_.y, scale_.z));
            const Matrix4x4& model = ConstructModelMatrix();

            const Sphere& meshBoundingSphere = mesh_->GetBoundingSphere();
            Vector4 transformedCenter = model * meshBoundingSphere.GetCenter();
            Sphere nodeBoundingSphere(Vector3(transformedCenter.x, transformedCenter.y, transformedCenter.z), meshBoundingSphere.GetRadius() * maxScaleValue);

            if (frustumPlanes.IsSphereOutside(nodeBoundingSphere)) {
                addMeshToRenderQueue = false;
            }
        }

        if (addMeshToRenderQueue) {
            renderQueue.AddNode(this);
        }
    }

	map<string, Node*>::const_iterator it = children_.begin();
	for (; it != children_.end(); ++it) {
		it->second->Render(frustumPlanes, useFrustumCulling, renderQueue);
	}
}

}
