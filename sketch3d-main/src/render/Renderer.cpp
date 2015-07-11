#include "render/Renderer.h"

#include "math/Constants.h"
#include "math/Sphere.h"

#include "render/OpenGL/RenderSystemOpenGL.h"

#include "system/Platform.h"

// Windows is the only OS where it makes sense to have DirectX
#if PLATFORM == PLATFORM_WIN32
#include "render/Direct3D9/RenderSystemDirect3D9.h"
#endif

#include "render/RenderStateCache.h"
#include "render/Texture2D.h"
#include "render/TextureManager.h"

#include "system/Logger.h"
#include "system/Window.h"

#include <math.h>

#include <FreeImage.h>

namespace Sketch3D {

Renderer Renderer::instance_;

Renderer::Renderer() : renderSystem_(nullptr), useFrustumCulling_(true), nearFrustumPlane_(0.0f), farFrustumPlane_(0.0f),
                       oldViewportX_(0), oldViewportY_(0), oldViewportWidth_(0), oldViewportHeight_(0)
{
}

Renderer::~Renderer() {
	delete renderSystem_;
}

Renderer* Renderer::GetInstance() {
	return &instance_;
}

bool Renderer::Initialize(RenderSystem_t renderSystem,
						  Window& window, const RenderParameters_t& renderParameters)
{
    renderParamters_ = renderParameters;

#if PLATFORM == PLATFORM_WIN32
	switch (renderSystem) {
		case OPENGL:    renderSystem_ = new RenderSystemOpenGL(window); break;
		case DIRECT3D9: renderSystem_ = new RenderSystemDirect3D9(window); break;
		default:
			Logger::GetInstance()->Error("Unknown render system");
			break;
	}
#elif PLATFORM == PLATFORM_LINUX
    renderSystem_ = new RenderSystemOpenGL(window);
#endif

	if (!renderSystem_->Initialize(renderParamters_)) {
        Logger::GetInstance()->Error("Couldn't initialize render system properly");
        return false;
    }

    SetDefaultRenderingValues();

    return true;
}

void Renderer::SetClearColor(float red, float green, float blue, float alpha) const {
	renderSystem_->SetClearColor(red, green, blue, alpha);
}

void Renderer::Clear(int buffer) const {
    renderSystem_->GetRenderStateCache()->ApplyClearStateChanges();
	renderSystem_->Clear(buffer);
}

void Renderer::StartRender() {
    renderSystem_->StartRender();
}

void Renderer::EndRender() {
	renderSystem_->EndRender();
}

void Renderer::Render() {
    /*
    // Commit the state changes
    RenderStateCache* renderStateCache = renderSystem_->GetRenderStateCache();
    renderStateCache->ApplyRenderStateChanges();

    // Draw the static batches first
    sceneTree_.RenderStaticBatches();

	// Populate the render queue with nodes from the scene tree
    FrustumPlanes_t frustumPlanes;
    if (useFrustumCulling_) {
        frustumPlanes = ExtractViewFrustumPlanes();
    }

    sceneTree_.Render(frustumPlanes, useFrustumCulling_, opaqueRenderQueue_, transparentRenderQueue_);

	// Draw the render queue contents
    opaqueRenderQueue_.Render();

    if (!transparentRenderQueue_.IsEmpty()) {
        renderStateCache->SetBlendingEquation(BLENDING_EQUATION_ADD);
        renderStateCache->EnableBlending(true);
        renderStateCache->ApplyRenderStateChanges();

        transparentRenderQueue_.Render();

        renderStateCache->EnableBlending(false);
    }
    */
}

void Renderer::PresentFrame() {
    renderSystem_->PresentFrame();
}

void Renderer::OrthoProjection(float left, float right,
							   float bottom, float top,
							   float nearPlane, float farPlane)
{
    projection_ = renderSystem_->OrthoProjection(left, right, bottom, top, nearPlane, farPlane);
	viewProjection_ = projection_ * view_;

    nearFrustumPlane_ = nearPlane;
    farFrustumPlane_ = farPlane;
}

void Renderer::PerspectiveProjection(float left, float right,
									 float bottom, float top,
									 float nearPlane, float farPlane)
{
    projection_ = renderSystem_->PerspectiveProjection(left, right, bottom, top, nearPlane, farPlane);
	viewProjection_ = projection_ * view_;

    nearFrustumPlane_ = nearPlane;
    farFrustumPlane_ = farPlane;
}

void Renderer::PerspectiveProjection(float fov, float aspect, float nearPlane,
									 float farPlane)
{
	float h = tan(fov * DEG_2_RAD_OVER_2) * nearPlane;
	float w = h * aspect;
	PerspectiveProjection(-w, w, -h, h, nearPlane, farPlane);
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

    SetViewMatrix(right, u, direction, position);
}

void Renderer::SetViewMatrix(const Vector3& right, const Vector3& up, const Vector3& look, const Vector3& position) {
    view_[0][0] = right.x;
    view_[0][1] = right.y;
    view_[0][2] = right.z;

    view_[1][0] = up.x;
    view_[1][1] = up.y;
    view_[1][2] = up.z;

    view_[2][0] = -look.x;
    view_[2][1] = -look.y;
    view_[2][2] = -look.z;

    view_[0][3] = -position.Dot(right);
    view_[1][3] = -position.Dot(up);
    view_[2][3] =  position.Dot(look);

    viewProjection_ = projection_ * view_;
}

void Renderer::SetRenderFillMode(FillMode_t mode) const {
	renderSystem_->SetRenderFillMode(mode);
}

void Renderer::SetViewport(size_t x, size_t y, size_t width, size_t height) {
    if (x != oldViewportX_ || y != oldViewportY_ || width != oldViewportWidth_ || height != oldViewportHeight_) {
        renderSystem_->SetViewport(x, y, width, height);

        oldViewportX_ = x;
        oldViewportY_ = y;
        oldViewportWidth_ = width;
        oldViewportHeight_ = height;
    }
}

void Renderer::EnableDepthTest(bool val) const {
    renderSystem_->EnableDepthTest(val);
}

void Renderer::EnableDepthWrite(bool val) const {
    renderSystem_->EnableDepthWrite(val);
}

void Renderer::EnableColorWrite(bool val) const {
    renderSystem_->EnableColorWrite(val);
}

void Renderer::SetDepthComparisonFunc(DepthFunc_t comparison) const {
    renderSystem_->SetDepthComparisonFunc(comparison);
}

void Renderer::SetCullingMethod(CullingMethod_t cullingMethod) {
    cullingMethod_ = cullingMethod;
    renderSystem_->SetCullingMethod(cullingMethod);
}

Shader* Renderer::CreateShader() const {
    return renderSystem_->CreateShader();
}

Texture2D* Renderer::CreateTexture2D() const {
    return renderSystem_->CreateTexture2D();
}

Texture2D* Renderer::CreateTexture2DFromFile(const string& filename, bool generateMipmaps) const {
    // Check cache first
    if (TextureManager::GetInstance()->CheckIfTextureLoaded(filename)) {
        return TextureManager::GetInstance()->LoadTextureFromCache(filename);
    }

    Texture2D* texture = renderSystem_->CreateTexture2D();
    texture->SetGenerateMipmaps(generateMipmaps);
    if (!texture->Load(filename)) {
        Logger::GetInstance()->Error("Couldn't create texture from file " + filename);
        delete texture;
        return nullptr;
    }

    return texture;
}

Texture3D* Renderer::CreateTexture3D() const {
    return renderSystem_->CreateTexture3D();
}

RenderTexture* Renderer::CreateRenderTexture(unsigned int width, unsigned int height, TextureFormat_t format) const {
    return renderSystem_->CreateRenderTexture(width, height, format);
}

void Renderer::BindScreenBuffer() const {
    renderSystem_->BindScreenBuffer();
}

void Renderer::EnableBlending(bool val) const {
    renderSystem_->EnableBlending(val);
}

void Renderer::SetBlendingEquation(BlendingEquation_t equation) const {
    renderSystem_->SetBlendingEquation(equation);
}

void Renderer::SetBlendingFactor(BlendingFactor_t srcFactor, BlendingFactor_t dstFactor) const {
    renderSystem_->SetBlendingFactor(srcFactor, dstFactor);
}

Vector3 Renderer::ScreenToWorldPoint(const Vector2& point) const {
    return renderSystem_->ScreenToWorldPoint(viewProjection_.Inverse(), point);
}

size_t Renderer::BindTexture(const Texture* texture) {
    return renderSystem_->BindTexture(texture);
}

void Renderer::BindShader(const Shader* shader) {
    renderSystem_->BindShader(shader);
}

FrustumPlanes_t Renderer::ExtractViewFrustumPlanes() const {
    return renderSystem_->ExtractViewFrustumPlanes(viewProjection_);
}

void Renderer::EnableFrustumCulling(bool val) {
    useFrustumCulling_ = val;
}

void Renderer::DrawTextBuffer(BufferObject* bufferObject, Texture2D* fontAtlas, const Vector3& textColor) {
    RenderStateCache* renderStateCache = renderSystem_->GetRenderStateCache();
    renderStateCache->EnableDepthTest(false);
    renderStateCache->ApplyRenderStateChanges();

    renderSystem_->DrawTextBuffer(bufferObject, fontAtlas, textColor);

    renderStateCache->EnableDepthTest(true);
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

float Renderer::GetNearFrustumPlane() const {
    return nearFrustumPlane_;
}

float Renderer::GetFarFrustumPlane() const {
    return farFrustumPlane_;
}

const SceneTree& Renderer::GetSceneTree() const {
	return sceneTree_;
}

SceneTree& Renderer::GetSceneTree() {
	return sceneTree_;
}

BufferObjectManager* Renderer::GetBufferObjectManager() const {
    return renderSystem_->GetBufferObjectManager();
}

RenderStateCache* Renderer::GetRenderStateCache() const {
    return renderSystem_->GetRenderStateCache();
}

size_t Renderer::GetScreenWidth() const {
    return renderSystem_->GetWidth();
}

size_t Renderer::GetScreenHeight() const {
    return renderSystem_->GetHeight();
}

CullingMethod_t Renderer::GetCullingMethod() const {
    return cullingMethod_;
}

void Renderer::SetDefaultRenderingValues() {
    // Some initial values
    PerspectiveProjection(45.0f, (float)renderParamters_.width / (float)renderParamters_.height, 1.0f, 1000.0f);
    CameraLookAt(Vector3::ZERO, Vector3::LOOK);

    SetCullingMethod(CullingMethod_t::BACK_FACE);
}

bool FrustumPlanes_t::IsSphereOutside(const Sphere& sphere) const {
    return sphere.IntersectsPlane(nearPlane) == RELATIVE_PLANE_POSITION_OUTSIDE ||
           sphere.IntersectsPlane(farPlane) == RELATIVE_PLANE_POSITION_OUTSIDE ||
           sphere.IntersectsPlane(leftPlane) == RELATIVE_PLANE_POSITION_OUTSIDE ||
           sphere.IntersectsPlane(rightPlane) == RELATIVE_PLANE_POSITION_OUTSIDE ||
           sphere.IntersectsPlane(bottomPlane) == RELATIVE_PLANE_POSITION_OUTSIDE ||
           sphere.IntersectsPlane(topPlane) == RELATIVE_PLANE_POSITION_OUTSIDE;
}

}
