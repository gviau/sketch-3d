#include "render/Texture.h"

#include "system/Logger.h"

namespace Sketch3D {
Texture::Texture(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable) : textureMap_(textureMap), textureFormat_(textureFormat) {
}

TextureMap* Texture::GetTextureMap() const {
    return textureMap_;
}

TextureFormat_t Texture::GetTextureFormat() const {
    return textureFormat_;
}

Texture1D::Texture1D(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable) : Texture(textureMap, textureFormat, dynamic, immutable) {
}

Texture2D::Texture2D(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable) : Texture(textureMap, textureFormat, dynamic, immutable) {
}

Texture3D::Texture3D(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable) : Texture(textureMap, textureFormat, dynamic, immutable) {
}

size_t GetRowPitch(size_t width, TextureFormat_t textureFormat) {
    size_t bytesPerPixel = 0;
    size_t padding = 0;

    switch (textureFormat) {
        case TextureFormat_t::GRAYSCALE:
            bytesPerPixel = 1;
            break;

        case TextureFormat_t::RGB24:
            bytesPerPixel = 3;
            padding = 1;
            break;

        case TextureFormat_t::RGBA32:
            bytesPerPixel = 4;
            break;

        case TextureFormat_t::R32F:
            bytesPerPixel = 4;
            break;

        case TextureFormat_t::RG32F:
            bytesPerPixel = 8;
            break;

        case TextureFormat_t::RGBA32F:
            bytesPerPixel = 16;
            break;

        case TextureFormat_t::R16F:
            bytesPerPixel = 2;
            break;

        case TextureFormat_t::RG16F:
            bytesPerPixel = 4;
            break;

        case TextureFormat_t::RGBA16F:
            bytesPerPixel = 8;
            break;

        case TextureFormat_t::DEPTH:
            bytesPerPixel = 4;
            break;
    }

    return (bytesPerPixel + padding) * width;
}

size_t Get2DSlicePitch(size_t width, size_t height, TextureFormat_t textureFormat) {
    size_t rowPitch = GetRowPitch(width, textureFormat);
    return height * rowPitch;
}
}
