#include "render/Node.h"

#include "render/BufferObject.h"
#include "render/Material.h"
#include "render/Mesh.h"
#include "render/ModelManager.h"
#include "render/Renderer.h"
#include "render/RenderQueue.h"
#include "render/RenderStateCache.h"
#include "render/Shader.h"
#include "render/SkinnedMesh.h"
#include "render/Texture2D.h"

#include <sstream>

namespace Sketch3D {

long long Node::nextNameIndex_ = 0;

Node::Node(Node* parent) : parent_(parent), mesh_(NULL), material_(NULL),
                           scale_(1.0f, 1.0f, 1.0), parentTransformation_(&Matrix4x4::IDENTITY),
                           needTransformationUpdate_(true), useInstancing_(false), isStatic_(false)
{
    ostringstream convert;
    convert << nextNameIndex_;
	name_ = "NewNode" + convert.str();
	nextNameIndex_ += 1;
}

Node::Node(const string& name, Node* parent) : name_(name), parent_(parent),
											   mesh_(NULL), material_(NULL),
											   scale_(1.0f, 1.0f, 1.0f), parentTransformation_(&Matrix4x4::IDENTITY),
                                               needTransformationUpdate_(true), useInstancing_(false), isStatic_(false)
{
}

Node::Node(const Vector3& position, const Vector3& scale,
		   const Quaternion& orientation, Node* parent) : parent_(parent),
                                                          position_(position),
														  scale_(scale),
														  orientation_(orientation),
														  mesh_(NULL),
														  material_(NULL),
                                                          parentTransformation_(&Matrix4x4::IDENTITY),
                                                          needTransformationUpdate_(true),
                                                          useInstancing_(false),
                                                          isStatic_(false)
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
                                                          parentTransformation_(&Matrix4x4::IDENTITY),
                                                          needTransformationUpdate_(true),
                                                          useInstancing_(false),
                                                          isStatic_(false)
{
}

Node::Node(const Node& src) : parent_(src.parent_),
                              position_(src.position_),
                              scale_(src.scale_),
                              orientation_(src.orientation_),
                              material_(src.material_),
                              parentTransformation_(&Matrix4x4::IDENTITY),
                              needTransformationUpdate_(true),
                              useInstancing_(false),
                              isStatic_(false)
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
    // Commit state changes
    Renderer::GetInstance()->GetRenderStateCache()->ApplyRenderStateChanges();

	Shader* shader = material_->GetShader();

    const Matrix4x4& projection = Renderer::GetInstance()->GetProjectionMatrix();
    const Matrix4x4& viewProjection = Renderer::GetInstance()->GetViewProjectionMatrix();
    const Matrix4x4& view = Renderer::GetInstance()->GetViewMatrix();

    // Setup the transformation matrix for this node
    const Matrix4x4& model = ConstructModelMatrix();
    Matrix4x4 modelViewProjection(viewProjection * model);
    Matrix4x4 modelView(view * model);

    // Set the uniform matrices
    Renderer::GetInstance()->BindShader(shader);
    shader->SetUniformMatrix4x4( GetBuiltinUniformName(BuiltinUniform_t::MODEL_VIEW_PROJECTION), modelViewProjection );
    shader->SetUniformMatrix4x4( GetBuiltinUniformName(BuiltinUniform_t::MODEL_VIEW), modelView );
    shader->SetUniformMatrix4x4( GetBuiltinUniformName(BuiltinUniform_t::MODEL), model );
    shader->SetUniformMatrix4x4( GetBuiltinUniformName(BuiltinUniform_t::VIEW), view );
    shader->SetUniformMatrix4x4( GetBuiltinUniformName(BuiltinUniform_t::PROJECTION), projection );

    // Get the rendering data
    BufferObject** bufferObjects;
    vector<SurfaceTriangles_t*> surfaces;
    mesh_->GetRenderInfo(bufferObjects, surfaces);

    material_->ApplyMaterial();

    // Render the mesh
    for (size_t i = 0; i < surfaces.size(); i++) {
        // Textures associated with the surface
        for (size_t j = 0; j < surfaces[i]->numTextures; j++) {
            Texture2D* texture = surfaces[i]->textures[j];
            if (texture != nullptr) {
                BuiltinUniform_t builtinUniformTexture = (BuiltinUniform_t)((size_t)BuiltinUniform_t::TEXTURE_0 + j);
                shader->SetUniformTexture( GetBuiltinUniformName(builtinUniformTexture), texture );
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

    node->parentTransformation_ = &cachedTransformation_;
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

    return (*parentTransformation_) * cachedTransformation_;
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

void Node::SetInstancing(bool val) {
    useInstancing_ = val;
    if (mesh_ != nullptr) {
        mesh_->PrepareInstancingData();
    }
}

void Node::SetStatic(bool val) {
    isStatic_ = val;
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

bool Node::UseInstancing() const {
    return useInstancing_;
}

bool Node::IsStatic() const {
    return isStatic_;
}

void Node::Render(const FrustumPlanes_t& frustumPlanes, bool useFrustumCulling, RenderQueue& opaqueRenderQueue, RenderQueue& transparentRenderQueue) {
    if (mesh_ != nullptr && !isStatic_) {
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
            if (material_->GetTransluencyType() == TRANSLUENCY_TYPE_OPAQUE) {
                opaqueRenderQueue.AddNode(this);
            } else {
                transparentRenderQueue.AddNode(this);
            }
        }
    }

	map<string, Node*>::const_iterator it = children_.begin();
	for (; it != children_.end(); ++it) {
        it->second->Render(frustumPlanes, useFrustumCulling, opaqueRenderQueue, transparentRenderQueue);
	}
}

}
