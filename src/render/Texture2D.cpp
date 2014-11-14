#include "render/Texture2D.h"

#include "render/TextureManager.h"
#include "system/Logger.h"

#include <FreeImage.h>

#include <cstring>

namespace Sketch3D {

uint32_t Texture2D::nextAvailableId_ = 0;

Texture2D::Texture2D() : Texture(), data_(nullptr), id_(MAX_TEXTURE_ID), fromCache_(false) {
    if (nextAvailableId_ == MAX_TEXTURE_ID) {
        Logger::GetInstance()->Error("Maximum number of textures created (" + to_string(MAX_TEXTURE_ID) + ")");
    } else {
        id_ = nextAvailableId_++;
    }
}

Texture2D::Texture2D(unsigned int width, unsigned int height,
					 FilterMode_t filterMode, WrapMode_t wrapMode,
					 TextureFormat_t format) : Texture(width, height, filterMode, wrapMode, format),
											   data_(nullptr), id_(MAX_TEXTURE_ID), fromCache_(false)
{
    if (nextAvailableId_ == MAX_TEXTURE_ID) {
        Logger::GetInstance()->Error("Maximum number of textures created (" + to_string(MAX_TEXTURE_ID) + ")");
    } else {
        id_ = nextAvailableId_++;
    }
}

Texture2D::~Texture2D() {
    if (!fromCache_) {
        free(data_);
    } else {
        TextureManager::GetInstance()->RemoveReferenceFromCache(filename_);
    }
}

bool Texture2D::Load(const string& filename) {
    if (filename_ == filename) {
        return true;
    }

    // Delete last texture if present
    if (data_ != nullptr) {
        if (fromCache_) {
            TextureManager::GetInstance()->RemoveReferenceFromCache(filename_);
        } else {
            free(data_);
            data_ = nullptr;
        }
    }

    // Create image then cache it for future loads
    FREE_IMAGE_FORMAT format = FIF_UNKNOWN;

    format = FreeImage_GetFileType(filename.c_str());
    if (format == FIF_UNKNOWN) {
        format = FreeImage_GetFIFFromFilename(filename.c_str());
    }

    if ((format == FIF_UNKNOWN) || !FreeImage_FIFSupportsReading(format)) {
        Logger::GetInstance()->Error("File format unsupported for image " + filename);
        return false;
    }

    FIBITMAP* dib = FreeImage_Load(format, filename.c_str());
    if (dib == nullptr) {
        Logger::GetInstance()->Error("Couldn't load image " + filename);
        return false;
    }

    width_ = FreeImage_GetWidth(dib);
    height_ = FreeImage_GetHeight(dib);
    size_t bpp = FreeImage_GetBPP(dib);

    filterMode_ = FILTER_MODE_NEAREST;
    wrapMode_ = WRAP_MODE_REPEAT;
    format_ = (bpp == 24) ? TEXTURE_FORMAT_BGR24 : TEXTURE_FORMAT_BGRA32;

    size_t bytesPerPixel = (format_ == TEXTURE_FORMAT_BGR24) ? 3 : 4;
    size_t size = width_ * height_ * bytesPerPixel;
    data_ = malloc(size);

    memcpy(data_, (void*)FreeImage_GetBits(dib), size);
    FreeImage_Unload(dib);

    if (!Create()) {
        Logger::GetInstance()->Error("Couldn't create texture handle for image " + filename);
        return false;
    }

    TextureManager::GetInstance()->CacheTexture(filename, this);
    filename_ = filename;
    fromCache_ = true;

    Logger::GetInstance()->Info("Successfully loaded image " + filename);
    return true;
}

bool Texture2D::SetPixelDataBytes(unsigned char* data, size_t width, size_t height) {
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

    SetPixelDataBytesImp(data);
    return true;
}

bool Texture2D::SetPixelDataFloats(float* data, size_t width, size_t height) {
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

    SetPixelDataFloatsImp(data);
    return true;
}

}
