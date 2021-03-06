#include "render/Texture2D.h"

#include "render/TextureManager.h"
#include "system/Logger.h"

#include <FreeImage.h>

#include <cstring>

namespace Sketch3D {

uint32_t Texture2D::nextAvailableId_ = 0;

Texture2D::Texture2D(bool generateMipmaps) : Texture(generateMipmaps), data_(nullptr), fromCache_(false) {
}

Texture2D::Texture2D(unsigned int width, unsigned int height, bool generateMipmaps,
					 FilterMode_t filterMode, WrapMode_t wrapMode,
					 TextureFormat_t format) : Texture(width, height, generateMipmaps, filterMode, wrapMode, format),
											   data_(nullptr), fromCache_(false)
{
}

Texture2D::~Texture2D() {
    if (!fromCache_) {
        free(data_);
    }
}

bool Texture2D::Load(const string& filename) {
    if (filename_ == filename) {
        return true;
    }

    // Delete last texture if present
    if (data_ != nullptr) {
        if (fromCache_) {
            TextureManager::GetInstance()->RemoveTextureReferenceFromCache(filename_);
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
    size_t pitch = FreeImage_GetPitch(dib);

    filterMode_ = FILTER_MODE_NEAREST;
    wrapMode_ = WRAP_MODE_REPEAT;

    size_t bytesPerPixel = 0;
    if (bpp == 8) {
        format_ = TEXTURE_FORMAT_GRAYSCALE;
        bytesPerPixel = 1;
    } else if (bpp == 24) {
        format_ = TEXTURE_FORMAT_RGB24;
        bytesPerPixel = 3;
    } else if (bpp == 32) {
        format_ = TEXTURE_FORMAT_RGBA32;
        bytesPerPixel = 4;
    }

    size_t size = height_ * pitch;
    unsigned char* data = (unsigned char*)malloc(size);
    FreeImage_ConvertToRawBits(data, dib, pitch, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, FALSE);

    // We store the texture data in rgba, where r is LSB and a is MSB
    size_t idx = 0;
    size_t pad = pitch - (width_ * bytesPerPixel);
    for (size_t y = 0; y < height_; y++) {
        for (size_t x = 0; x < width_; x++) {
            unsigned char tmp = data[idx];
            data[idx    ] = data[idx + 2];
            data[idx + 2] = tmp;
            idx += bytesPerPixel;
        }

        // Needed so that the world doesn't destroy itself (real reason is that we need to jump above some bytes because
        // free image will add some bytes to round up to a number of bytes divisible by 4)
        idx += pad;
    }

    FreeImage_Unload(dib);
    data_ = (void*)data;

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
