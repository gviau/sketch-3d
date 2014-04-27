#include "render/RenderSystem.h"

#include "system/Window.h"

namespace Sketch3D {

RenderSystem::RenderSystem(Window& window) : window_(window) {
    windowHandle_ = window_.GetHandle();
    width_ = window_.GetWidth();
    height_ = window_.GetHeight();
    windowed_ = window_.IsWindowed();
}

}
