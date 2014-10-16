#include "render/Texture3D.h"

#include "system/Logger.h"

namespace Sketch3D {

Texture3D::Texture3D() : Texture(), depth_(0), data_(nullptr) {
}

Texture3D::Texture3D(unsigned int width, unsigned int height, unsigned int depth,
                     FilterMode_t filterMode, WrapMode_t wrapMode, TextureFormat_t format) :
        Texture(width, height, filterMode, wrapMode, format), depth_(depth), data_(nullptr)
{
}

Texture3D::~Texture3D() {
    free(data_);
}

bool Texture3D::SetPixelDataBytes(unsigned char* data, size_t width, size_t height, size_t depth) {
    if (format_ >= TEXTURE_FORMAT_R32F || format_ == TEXTURE_FORMAT_DEPTH) {
        Logger::GetInstance()->Warning("Operation not supported for format different than byte texture format in SetPixelDataBytes");
        return false;
    }
    
    if (width_ != width) {
        Logger::GetInstance()->Warning("New pixel data isn't of appropriate width in SetPixelDataBytes");
        return false;
    }

    if (height_ != height) {
        Logger::GetInstance()->Warning("New pixel data isn't of appropriate height in SetPixelDataBytes");
        return false;
    }

    if (depth_ != depth) {
        Logger::GetInstance()->Warning("New pixel data isn't of appropriate depth in SetPixelDataBytes");
        return false;
    }

    SetPixelDataBytesImp(data);
    return true;
}

bool Texture3D::SetPixelDataFloats(float* data, size_t width, size_t height, size_t depth) {
    if (format_ < TEXTURE_FORMAT_R32F || format_ == TEXTURE_FORMAT_DEPTH) {
        Logger::GetInstance()->Warning("Operation not supported for format different than floating point texture format in SetPixelDataFloats");
        return false;
    }
    
    if (width_ != width) {
        Logger::GetInstance()->Warning("New pixel data isn't of appropriate width in SetPixelDataFloats");
        return false;
    }

    if (height_ != height) {
        Logger::GetInstance()->Warning("New pixel data isn't of appropriate height in SetPixelDataFloats");
        return false;
    }

    if (depth_ != depth) {
        Logger::GetInstance()->Warning("New pixel data isn't of appropriate depth in SetPixelDataBytes");
        return false;
    }

    SetPixelDataFloatsImp(data);
    return true;
}

}