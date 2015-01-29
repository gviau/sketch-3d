#include "render/Renderer.h"

#include "math/Constants.h"

#include "render/OpenGL/RenderSystemOpenGL.h"

// Windows is the only OS where it makes sense to have DirectX
#if defined(__WIN32__) || defined(_WIN32)
#include "render/Direct3D9/RenderSystemDirect3D9.h"
#endif

#include "render/Texture2D.h"
#include "render/TextureManager.h"

#include "system/Logger.h"
#include "system/Window.h"

#include <math.h>

#include <FreeImage.h>

namespace Sketch3D {

Renderer Renderer::instance_;

Renderer::Renderer() : renderSystem_(nullptr), boundShader_(nullptr) {
}

Renderer::~Renderer() {
    TextureUnitNode_t* node = head_;
    TextureUnitNode_t* toDelete = nullptr;
    while (node != nullptr) {
        toDelete = node;
        node = node->next;
        delete toDelete;
    }

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
            renderSystem_ = new RenderSystemDirect3D9(window);			
			break;

		default:
			Logger::GetInstance()->Error("Unknown render system");
			break;
	}

	if (!renderSystem_->Initialize()) {
        Logger::GetInstance()->Error("Couldn't initialize render system properly");
        return false;
    }

    // Some initial values
    PerspectiveProjection(45.0f, (float)window.GetWidth() / (float)window.GetHeight(), 1.0f, 1000.0f);
    CameraLookAt(Vector3::ZERO, Vector3::LOOK);

    SetCullingMethod(CULLING_METHOD_BACK_FACE);
    EnableDepthTest(true);
    SetDepthComparisonFunc(DEPTH_FUNC_LESS);
    SetRenderFillMode(RENDER_MODE_FILL);

    // Construct the texture cache
    const DeviceCapabilities_t* deviceCapabilities = renderSystem_->GetDeviceCapabilities();
    head_ = new TextureUnitNode_t;
    head_->textureUnit = 0;
    head_->prev = nullptr;
    tail_ = head_;

    for (int i = 1; i < deviceCapabilities->maxActiveTextures_ + 1; i++) {
        tail_->next = new TextureUnitNode_t;
        tail_->next->prev = tail_;
        tail_ = tail_->next;
        tail_->textureUnit = i;
    }
    tail_->next = nullptr;

    return true;
}

void Renderer::SetClearColor(float red, float green, float blue, float alpha) const {
	renderSystem_->SetClearColor(red, green, blue, alpha);
}

void Renderer::Clear(int buffer) const {
	renderSystem_->Clear(buffer);
}

void Renderer::StartRender() {
    renderSystem_->StartRender();
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

void Renderer::PresentFrame() {
    renderSystem_->PresentFrame();
}

void Renderer::OrthoProjection(float left, float right,
							   float bottom, float top,
							   float nearPlane, float farPlane)
{
    projection_ = renderSystem_->OrthoProjection(left, right, bottom, top, nearPlane, farPlane);
	viewProjection_ = projection_ * view_;
}

void Renderer::PerspectiveProjection(float left, float right,
									 float bottom, float top,
									 float nearPlane, float farPlane)
{
    projection_ = renderSystem_->PerspectiveProjection(left, right, bottom, top, nearPlane, farPlane);
	viewProjection_ = projection_ * view_;
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

void Renderer::SetRenderFillMode(RenderMode_t mode) const {
	renderSystem_->SetRenderFillMode(mode);
}

void Renderer::SetViewport(size_t x, size_t y, size_t width, size_t height) {
    renderSystem_->SetViewport(x, y, width, height);
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

void Renderer::SetCullingMethod(CullingMethod_t cullingMethod) const {
    renderSystem_->SetCullingMethod(cullingMethod);
}

Shader* Renderer::CreateShader(const string& vertexFilename, const string& fragmentFilename) const {
    return renderSystem_->CreateShader(vertexFilename, fragmentFilename);
}

Texture2D* Renderer::CreateTexture2D() const {
    return renderSystem_->CreateTexture2D();
}

Texture2D* Renderer::CreateTexture2DFromFile(const string& filename, bool generateMipmaps) const {
    // Check cache first
    if (TextureManager::GetInstance()->CheckIfTextureLoaded(filename)) {
        return TextureManager::GetInstance()->LoadFromCache(filename);
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
    // Verify if the node is already bound
    size_t textureUnit = 0;
    TextureUnitNode_t* nodeToUse = head_;
    TextureCache_t::iterator it = textureCache_.find(texture);

    if (it != textureCache_.end()) {
        nodeToUse = it->second;
    } else {
        // Bind it otherwise
        renderSystem_->BindTexture(texture, nodeToUse->textureUnit);
        textureCache_[texture] = nodeToUse;
    }
    textureUnit = nodeToUse->textureUnit;

    // Put the node to use at the end of the list if it isn't there yet
    if (tail_ != nodeToUse) {
        tail_->next = nodeToUse;

        // Remove that node from the list since its now a duplicate
        if (nodeToUse->prev) {
            nodeToUse->prev->next = nodeToUse->next;
        }

        if (nodeToUse->next) {
            nodeToUse->next->prev = nodeToUse->prev;
        }

        // Advance head if needed
        if (nodeToUse == head_) {
            head_ = head_->next;
        }

        tail_->next->prev = tail_;
        tail_ = tail_->next;
        tail_->next = nullptr;
    }

    return textureUnit;
}

void Renderer::BindShader(const Shader* shader) {
    if (shader != boundShader_) {
        renderSystem_->BindShader(shader);
        boundShader_ = shader;
    }
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

BufferObjectManager* Renderer::GetBufferObjectManager() const {
    return renderSystem_->GetBufferObjectManager();
}

}
