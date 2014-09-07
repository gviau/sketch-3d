#include "render/Texture2D.h"

#include "render/ResourceManager.h"

#include "system/Logger.h"

namespace Sketch3D {

uint32_t Texture2D::nextAvailableId_ = 0;

Texture2D::Texture2D() : Texture(), format_(TEXTURE_FORMAT_RGB24), data_(nullptr), id_(MAX_TEXTURE_ID) {
    if (nextAvailableId_ == MAX_TEXTURE_ID) {
        Logger::GetInstance()->Error("Maximum number of textures created (" + to_string(MAX_TEXTURE_ID) + ")");
    } else {
        id_ = nextAvailableId_++;
    }
}

Texture2D::Texture2D(unsigned int width, unsigned int height,
					 FilterMode_t filterMode, WrapMode_t wrapMode,
					 TextureFormat_t format) : Texture(width, height, filterMode, wrapMode),
											   format_(format), data_(nullptr), id_(MAX_TEXTURE_ID)
{
    if (nextAvailableId_ == MAX_TEXTURE_ID) {
        Logger::GetInstance()->Error("Maximum number of textures created (" + to_string(MAX_TEXTURE_ID) + ")");
    } else {
        id_ = nextAvailableId_++;
    }
}

Texture2D::~Texture2D() {
}

void Texture2D::SetPixelData(unsigned char* data, size_t width, size_t height) {
    if (format_ != TEXTURE_FORMAT_RGBA32) {
        Logger::GetInstance()->Warning("Operation not supported for format different than RGBA32 in SetPixelData");
        return;
    }
    
    if (width_ != width) {
        Logger::GetInstance()->Warning("New pixel data isn't of appropriate width in SetPixelData");
        return;
    }

    if (height_ != height) {
        Logger::GetInstance()->Warning("New pixel data isn't of appropriate height in SetPixelData");
        return;
    }

    SetPixelDataImp(data);
}

void Texture2D::SetTextureFormat(TextureFormat_t format) {
	format_ = format;
}

TextureFormat_t Texture2D::GetTextureFormat() const {
	return format_;
}

}