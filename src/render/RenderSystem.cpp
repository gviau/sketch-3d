#include "render/RenderSystem.h"

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

}
