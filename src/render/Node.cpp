#include "render/Node.h"

#include "render/Material.h"
#include "render/Mesh.h"
#include "render/Renderer.h"
#include "render/Shader.h"

#include <sstream>

// TEMP
#include "render/OpenGL/gl/glew.h"
#include <gl/GL.h>

namespace Sketch3D {

long long Node::nextNameIndex_ = 0;

Node::Node(Node* parent) : parent_(parent), mesh_(NULL), material_(NULL),
						   scale_(1.0f, 1.0f, 1.0)
{
    ostringstream convert;
    convert << nextNameIndex_;
	name_ = "NewNode" + convert.str();
	nextNameIndex_ += 1;
}

Node::Node(const string& name, Node* parent) : name_(name), parent_(parent),
											   mesh_(NULL), material_(NULL),
											   scale_(1.0f, 1.0f, 1.0f)
{
}

Node::Node(const Vector3& position, const Vector3& scale,
		   const Quaternion& orientation, Node* parent) : parent_(parent),
                                                          position_(position),
														  scale_(scale),
														  orientation_(orientation),
														  mesh_(NULL),
														  material_(NULL)
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
														  material_(NULL)
{
}

Node::~Node() {
}

void Node::Render() const {
	ConcreteRender();
}

void Node::ConcreteRender() const {
	Shader* shader = material_->GetShader();
    shader->SetActive(true);

	const Matrix4x4& viewProjection = Renderer::GetInstance()->GetViewProjectionMatrix();
	Matrix4x4 model;

	// Setup the transformation matrix for this node
	model[0][0] = scale_.x;
	model[1][1] = scale_.y;
	model[2][2] = scale_.z;

	Matrix4x4 rotation;
	orientation_.ToRotationMatrix(rotation);
	model = rotation * model;

	model[0][3] = position_.x;
	model[1][3] = position_.y;
	model[2][3] = position_.z;

	Matrix4x4 modelViewProjection = viewProjection * model;
	Matrix3x3 modelView = Renderer::GetInstance()->GetViewMatrix() * model;
	shader->SetUniformMatrix4x4("modelViewProjection", modelViewProjection);
	shader->SetUniformMatrix3x3("modelView", modelView);

	// Get the rendering data
    vector<LoadedModel_t*>* modelData;
    unsigned int* bufferObjects;
    mesh_->GetRenderInfo(modelData, bufferObjects);

    // textures
    const vector<vector<Texture2D*>>* textures = material_->GetTextures();

    // TEMP
    // Render the mesh
    for (size_t i = 0; i < modelData->size(); i++) {
        vector<Texture2D*> meshTextures = (*textures)[i];
        for (size_t j = 0; j < meshTextures.size(); j++) {
            Texture2D* texture = meshTextures[j];
            if (texture != nullptr) {
                texture->Bind(j);
                shader->SetUniformTexture("texture" + to_string(j), j);
            }
        }

        glBindVertexArray(bufferObjects[i]);
	    glDrawElements(GL_TRIANGLES, (*modelData)[i]->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void Node::Translate(const Vector3& translation) {
	position_ += translation;
}

void Node::Scale(const Vector3& scale) {
	scale_.x *= scale.x;
	scale_.y *= scale.y;
	scale_.z *= scale.z;
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
	orientation_ = rot;
}

void Node::SetParent(Node* parent) {
	parent_ = parent;
}

void Node::SetPosition(const Vector3& position) {
	position_ = position;
}

void Node::SetScale(const Vector3& scale) {
	scale_ = scale;
}

void Node::SetOrientation(const Quaternion& orientation) {
	orientation_ = orientation;
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

}
