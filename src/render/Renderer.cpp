#include "render/Renderer.h"

#include "math/Constants.h"
#include "render/OpenGL/RenderSystemOpenGL.h"
#include "system/Logger.h"
#include "system/Window.h"

#include <math.h>

namespace Sketch3D {

Renderer Renderer::instance_;

Renderer::Renderer() : renderSystem_(NULL) {
}

Renderer::~Renderer() {
	delete renderSystem_;
}

Renderer* Renderer::GetInstance() {
	return &instance_;
}

bool Renderer::Initialize(RenderSystem_t renderSystem,
						  Window& window)
{
	switch (renderSystem) {
		case RENDER_SYSTEM_OPENGL:
			renderSystem_ = new RenderSystemOpenGL(window);
			break;

		case RENDER_SYSTEM_DIRECT3D9:
			Logger::GetInstance()->Error("Direct3D9 still not supported");
			break;

		default:
			Logger::GetInstance()->Error("Unknown render system");
			break;
	}

	return renderSystem_->Initialize();
}

void Renderer::SetClearColor(float red, float green, float blue, float alpha) {
	renderSystem_->SetClearColor(red, green, blue, alpha);
}

bool Renderer::BeginRender() {
	return renderSystem_->BeginRender();
}

void Renderer::EndRender() {
	renderSystem_->EndRender();
}

void Renderer::Render() {
	// Render all the nodes in the scene tree. This call prepares the data for
	// the rendering process.
	sceneTree_.Render();

	// The RenderSystem draws what it received from the scene tree.
	renderSystem_->Render();
}

void Renderer::OrthoProjection(float left, float right,
							   float bottom, float top,
							   float near, float far)
{
	float dx = right - left;
	float dy = top - bottom;
	float dz = near - far;

	projection_ = Matrix4x4::IDENTITY;
	projection_[0][0] = 2.0f / dx;
	projection_[1][1] = 2.0f / dy;
	projection_[2][2] = 2.0f / dz;
	projection_[0][3] = (left - right) / dx;
	projection_[1][3] = (bottom - top) / dy;
	projection_[2][3] = dz / (far - near);

	viewProjection_ = projection_ * view_;
}

void Renderer::PerspectiveProjection(float left, float right,
									 float bottom, float top,
									 float near, float far)
{
	float dx = right - left;
	float dy = top - bottom;
	float dz = near - far;
	float z2 = 2.0f * near;

	projection_ = Matrix4x4::IDENTITY;
	projection_[0][0] = 2.0f * near / dx;
	projection_[1][1] = 2.0f * near / dy;
	projection_[0][2] = (right + left) / dx;
	projection_[1][2] = (top + bottom) / dy;
	projection_[2][2] = (far + near) / dz;
	projection_[3][2] = -1.0f;
	projection_[2][3] = 2.0f * near * far / dz;
	projection_[3][3] = 0.0f;

	viewProjection_ = projection_ * view_;
}

void Renderer::PerspectiveProjection(float fov, float aspect, float near,
									 float far)
{
	float h = tan(fov * DEG_2_RAD_OVER_2) * near;
	float w = h * aspect;
	PerspectiveProjection(-w, w, -h, h, near, far);
}

void Renderer::CameraLookAt(const Vector3& position, const Vector3& point,
							const Vector3& up)
{
	Vector3 direction = point - position;
	direction.Normalize();

	Vector3 right = direction.Cross(up);
	right.Normalize();

	Vector3 u = right.Cross(direction);
	u.Normalize();

	view_[0][0] = right.x;
	view_[0][1] = right.y;
	view_[0][2] = right.z;
	
	view_[1][0] = u.x;
	view_[1][1] = u.y;
	view_[1][2] = u.z;
		
	view_[2][0] = -direction.x;
	view_[2][1] = -direction.y;
	view_[2][2] = -direction.z;

	view_[0][3] = -position.Dot(right);
	view_[1][3] = -position.Dot(u);
	view_[2][3] = position.Dot(direction);

	viewProjection_ = projection_ * view_;
}

void Renderer::SetRenderFillMode(RenderMode_t mode) {
	renderSystem_->SetRenderFillMode(mode);
}

const Matrix4x4& Renderer::GetProjectionMatrix() const {
	return projection_;
}

const Matrix4x4& Renderer::GetViewMatrix() const {
	return view_;
}

const Matrix4x4& Renderer::GetViewProjectionMatrix() const {
	return viewProjection_;
}

const SceneTree& Renderer::GetSceneTree() const {
	return sceneTree_;
}

SceneTree& Renderer::GetSceneTree() {
	return sceneTree_;
}

}
