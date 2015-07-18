#include "render/RenderTarget.h"

namespace Sketch3D {
RenderTarget::RenderTarget(size_t width, size_t height, TextureFormat_t format) : width_(width), height_(height), format_(format) {
}

size_t RenderTarget::GetWidth() const {
    return width_;
}

size_t RenderTarget::GetHeight() const {
    return height_;
}

TextureFormat_t RenderTarget::GetFormat() const {
    return format_;
}

DepthStencilTarget::DepthStencilTarget(size_t width, size_t height) : width_(width), height_(height) {
}

size_t DepthStencilTarget::GetWidth() const {
    return width_;
}

size_t DepthStencilTarget::GetHeight() const {
    return height_;
}
}