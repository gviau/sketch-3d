#include "render/RenderTarget.h"

namespace Sketch3D {

size_t RenderTarget::GetWidth() const {
    return width_;
}

size_t RenderTarget::GetHeight() const {
    return height_;
}

TextureFormat_t RenderTarget::GetFormat() const {
    return format_;
}

size_t DepthStencilTarget::GetWidth() const {
    return width_;
}

size_t DepthStencilTarget::GetHeight() const {
    return height_;
}
}