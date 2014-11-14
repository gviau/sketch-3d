#include "render/RenderSystem.h"

#include "math/Matrix4x4.h"
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include "render/Shader.h"

#include "system/Window.h"

namespace Sketch3D {

RenderSystem::RenderSystem(Window& window) : window_(window) {
    windowHandle_ = window_.GetHandle();
    width_ = window_.GetWidth();
    height_ = window_.GetHeight();
    windowed_ = window_.IsWindowed();
}

RenderSystem::~RenderSystem() {
    for (size_t i = 0; i < shaders_.size(); i++) {
        delete shaders_[i];
    }
}

Vector3 RenderSystem::ScreenToWorldPoint(Matrix4x4& inversedViewProjection, const Vector2& point) const {
    // Transformation of normalized coordinates (-1, 1)
    Vector4 transformation(point.x / width_ * 2.0f - 1.0f, point.y / height_ * 2.0f - 1.0f, 1.0f, 1.0f);
    transformation *= inversedViewProjection;

    // Invert
    float inversion = 1.0f / transformation.w;

    return Vector3(transformation.x * inversion, transformation.y * inversion, transformation.z * inversion);
}

}
