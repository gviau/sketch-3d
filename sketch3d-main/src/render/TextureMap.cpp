#include "render/TextureMap.h"

namespace Sketch3D {
TextureMap::TextureMap(unsigned char* data, size_t width, size_t height, size_t depth) : data_(data), width_(width), height_(height), depth_(depth) {
}

unsigned char* TextureMap::GetData() const {
    return data_;
}

unsigned char*& TextureMap::GetData() {
    return data_;
}

size_t TextureMap::GetWidth() const {
    return width_;
}

size_t TextureMap::GetHeight() const {
    return height_;
}

size_t TextureMap::GetDepth() const {
    return depth_;
}
}