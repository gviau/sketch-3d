#include "render/Direct3D9/Texture2DDirect3D9.h"

#include "system/Logger.h"

#include <d3d9.h>
#include <d3dx9.h>

namespace Sketch3D {

Texture2DDirect3D9::Texture2DDirect3D9(IDirect3DDevice9* device) : device_(device), texture_(nullptr) {
}

Texture2DDirect3D9::Texture2DDirect3D9(IDirect3DDevice9* device, unsigned int width, unsigned int height,
                                       FilterMode_t filter, WrapMode_t wrapMode, TextureFormat_t format) :
            Texture2D(width, height, filter, wrapMode, format), device_(device), texture_(nullptr)
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
        case TEXTURE_FORMAT_BGR24:
        case TEXTURE_FORMAT_RGB24:
            format = D3DFMT_R8G8B8;
            bpp = 3;
            break;

        case TEXTURE_FORMAT_BGRA32:
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
            bpp = 3;
            break;

        case TEXTURE_FORMAT_DEPTH:
            format = D3DFMT_D16;
            bpp = 0;
            break;

        default:
            Logger::GetInstance()->Warning("Unsupported texture format");
            return false;
    }

    device_->CreateTexture(width_, height_, 1, 0, format, D3DPOOL_MANAGED, &texture_, nullptr);

    if (data_ != nullptr) {
        D3DLOCKED_RECT lockedRect;
        texture_->LockRect(0, &lockedRect, nullptr, D3DLOCK_DISCARD);

        memcpy(lockedRect.pBits, data_, width_ * height_ * bpp);

        texture_->UnlockRect(0);
    }

    return true;
}

const void* Texture2DDirect3D9::GetData() const {
    return nullptr;
}

void Texture2DDirect3D9::SetPixelDataBytesImp(unsigned char* data) {
}

void Texture2DDirect3D9::SetPixelDataFloatsImp(float* data) {
}

}