#include "render/RenderSystem.h"

#include "math/Matrix4x4.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include "render/BufferObjectManager.h"
#include "render/RenderStateCache.h"
#include "render/RenderTexture.h"
#include "render/Shader.h"

#include "system/Window.h"

namespace Sketch3D {

RenderSystem::RenderSystem(Window& window) : window_(window), boundShader_(nullptr), bufferObjectManager_(nullptr), textShader_(nullptr) {
    windowHandle_ = window_.GetHandle();
    width_ = window_.GetWidth();
    height_ = window_.GetHeight();
    windowed_ = window_.IsWindowed();
}

RenderSystem::~RenderSystem() {
}

void RenderSystem::SetRenderFillMode(FillMode_t mode) {
    renderStateCache_->SetRenderFillMode(mode);
}

void RenderSystem::EnableDepthTest(bool val) {
    renderStateCache_->EnableDepthTest(val);
}

void RenderSystem::EnableDepthWrite(bool val) {
    renderStateCache_->EnableDepthWrite(val);
}

void RenderSystem::EnableColorWrite(bool val) {
    renderStateCache_->EnableColorWrite(val);
}

void RenderSystem::SetDepthComparisonFunc(DepthFunc_t func) {
    renderStateCache_->SetDepthComparisonFunc(func);
}

void RenderSystem::SetCullingMethod(CullingMethod_t method) {
    renderStateCache_->SetCullingMethod(method);
}

void RenderSystem::EnableBlending(bool val) {
    renderStateCache_->EnableBlending(val);
}

void RenderSystem::SetBlendingEquation(BlendingEquation_t equation) {
    renderStateCache_->SetBlendingEquation(equation);
}

void RenderSystem::SetBlendingFactor(BlendingFactor_t srcFactor, BlendingFactor_t dstFactor) {
    renderStateCache_->SetBlendingFactor(srcFactor, dstFactor);
}

Vector3 RenderSystem::ScreenToWorldPoint(const Matrix4x4& inversedViewProjection, const Vector2& point) const {
    // Transformation of normalized coordinates (-1, 1)
    Vector3 transformation(2.0f * point.x / width_ - 1.0f, -2.0f * point.y / height_ + 1.0f, 0.0f);
    return Matrix3x3(inversedViewProjection) * transformation;
}

size_t RenderSystem::BindTexture(const Texture* texture) {
    return 0;
}

void RenderSystem::DrawTextBuffer(BufferObject* bufferObject, Texture2D* fontAtlas, const Vector3& textColor) {
    Renderer::GetInstance()->BindShader(textShader_);
    //textShader_->SetUniformTexture("fontAtlas", fontAtlas);
    //textShader_->SetUniformVector3("textColor", textColor);
    bufferObject->Render();
}

BufferObjectManager* RenderSystem::GetBufferObjectManager() const {
    return bufferObjectManager_;
}

RenderStateCache* RenderSystem::GetRenderStateCache() const {
    return renderStateCache_;
}

void RenderSystem::FreeRenderSystem() {
    for (size_t i = 0; i < shaders_.size(); i++) {
        delete shaders_[i];
    }

    for (size_t i = 0; i < renderTextures_.size(); i++) {
        delete renderTextures_[i];
    }

    delete bufferObjectManager_;
    delete renderStateCache_;
}

}
