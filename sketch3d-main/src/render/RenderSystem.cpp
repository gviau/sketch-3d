#include "render/RenderSystem.h"

#include "math/Matrix4x4.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include "render/BufferObjectManager.h"
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
    for (size_t i = 0; i < shaders_.size(); i++) {
        delete shaders_[i];
    }

    for (size_t i = 0; i < renderTextures_.size(); i++) {
        delete renderTextures_[i];
    }

    delete bufferObjectManager_;
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
    textShader_->SetUniformTexture("fontAtlas", fontAtlas);
    textShader_->SetUniformVector3("textColor", textColor);
    bufferObject->Render();
}

BufferObjectManager* RenderSystem::GetBufferObjectManager() const {
    return bufferObjectManager_;
}

}
