#include "render/Direct3D9/Texture2DDirect3D9.h"

#include "system/Logger.h"

#include <d3d9.h>
#include <d3dx9.h>

namespace Sketch3D {

Texture2DDirect3D9::Texture2DDirect3D9(IDirect3DDevice9* device, bool generateMipmaps) : Texture2D(generateMipmaps), device_(device), texture_(nullptr) {
}

Texture2DDirect3D9::Texture2DDirect3D9(IDirect3DDevice9* device, unsigned int width, unsigned int height, bool generateMipmaps,
                                       FilterMode_t filter, WrapMode_t wrapMode, TextureFormat_t format) :
            Texture2D(width, height, generateMipmaps, filter, wrapMode, format), device_(device), texture_(nullptr)
{
}

Texture2DDirect3D9::~Texture2DDirect3D9() {
    if (texture_ != nullptr) {
        texture_->Release();
    }
}

bool Texture2DDirect3D9::Create() {
    if (texture_ != nullptr) {
        texture_->Release();
        texture_ = nullptr;
    }

    D3DFORMAT format;
    size_t bpp;

    switch (format_) {
        case TEXTURE_FORMAT_GRAYSCALE:
            format = D3DFMT_A8;
            bpp = 1;
            break;

        case TEXTURE_FORMAT_RGB24:
            format = D3DFMT_R8G8B8;
            bpp = 3;
            break;

        case TEXTURE_FORMAT_RGBA32:
            format = D3DFMT_A8R8G8B8;
            bpp = 4;
            break;

        case TEXTURE_FORMAT_R32F:
            format = D3DFMT_R32F;
            bpp = 1;
            break;

        case TEXTURE_FORMAT_RG32F:
            format = D3DFMT_G32R32F;
            bpp = 2;
            break;

        case TEXTURE_FORMAT_RGBA32F:
            format = D3DFMT_A32B32G32R32F;
            bpp = 4;
            break;

        case TEXTURE_FORMAT_R16F:
            format = D3DFMT_R16F;
            bpp = 1;
            break;

        case TEXTURE_FORMAT_RG16F:
            format = D3DFMT_G16R16F;
            bpp = 2;
            break;

        case TEXTURE_FORMAT_RGBA16F:
            format = D3DFMT_A16B16G16R16F;
            bpp = 4;
            break;

        case TEXTURE_FORMAT_DEPTH:
            format = D3DFMT_D32;
            bpp = 0;
            break;

        default:
            Logger::GetInstance()->Warning("Unsupported texture format");
            return false;
    }

    DWORD flags = 0;
    if (generateMipmaps_ && format_ != TEXTURE_FORMAT_DEPTH) {
        flags = D3DUSAGE_AUTOGENMIPMAP;
    }
    D3DXCreateTexture(device_, width_, height_, flags, 0, format, D3DPOOL_MANAGED, &texture_);

    if (texture_ == nullptr) {
        Logger::GetInstance()->Error("Couldn't create texture");
        return false;
    }
    
    if (data_ != nullptr) {
        D3DLOCKED_RECT lockedRect;

        if (SUCCEEDED(texture_->LockRect(0, &lockedRect, nullptr, 0))) {
            unsigned char* rect = static_cast<unsigned char*>(lockedRect.pBits);
            unsigned char* data = static_cast<unsigned char*>(data_);

            size_t rect_idx = 0;
            size_t data_idx = 0;
            size_t pitch = ((((bpp * 8 * width_) + 31) / 32) * 4);
            size_t pad = pitch - (width_ * bpp);

            for (size_t y = 0; y < height_; y++) {
                for (size_t x = 0; x < width_; x++) {
                    rect[rect_idx    ] = data[data_idx + 2];
                    rect[rect_idx + 1] = data[data_idx + 1];
                    rect[rect_idx + 2] = data[data_idx    ];

                    if (bpp == 4) {
                        rect[rect_idx + 3] = data[data_idx + 3];
                    }

                    rect_idx += 4;
                    data_idx += bpp;
                }

                data_idx += pad;
            }

            texture_->UnlockRect(0);
        }
    }

    if (generateMipmaps_) {
        texture_->GenerateMipSubLevels();
    }
    
    return true;
}

const void* Texture2DDirect3D9::GetData() const {
    return nullptr;
}

void Texture2DDirect3D9::SetFilterModeImpl() const {
}

void Texture2DDirect3D9::SetWrapModeImpl() const {
}

void Texture2DDirect3D9::SetPixelDataBytesImp(unsigned char* data) {
    // TODO
    // FIX THAT
    D3DLOCKED_RECT lockedRect;
    size_t bpp = 0;

    switch (format_) {
        case TEXTURE_FORMAT_GRAYSCALE:
            bpp = 1;
            break;

        case TEXTURE_FORMAT_RGB24:
            bpp = 3;
            break;

        case TEXTURE_FORMAT_RGBA32:
            bpp = 4;
            break;

        case TEXTURE_FORMAT_R32F:
            bpp = 1;
            break;

        case TEXTURE_FORMAT_RG32F:
            bpp = 2;
            break;

        case TEXTURE_FORMAT_RGBA32F:
            bpp = 4;
            break;

        case TEXTURE_FORMAT_DEPTH:
            bpp = 0;
            break;

        default:
            Logger::GetInstance()->Warning("Unsupported texture format");
            return;
    }

    if (SUCCEEDED(texture_->LockRect(0, &lockedRect, nullptr, 0))) {
        unsigned char* rect = static_cast<unsigned char*>(lockedRect.pBits);

        memcpy(rect, data, width_ * height_ * bpp);
    
        texture_->UnlockRect(0);
    }
}

void Texture2DDirect3D9::SetPixelDataFloatsImp(float* data) {
}

}