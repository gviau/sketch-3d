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

    D3DXCreateTexture(device_, width_, height_, 1, 0, format, D3DPOOL_MANAGED, &texture_);
    if (texture_ == nullptr) {
        Logger::GetInstance()->Error("Couldn't create texture");
        return false;
    }

    if (data_ != nullptr) {
        D3DLOCKED_RECT lockedRect;
        if (SUCCEEDED(texture_->LockRect(0, &lockedRect, nullptr, 0))) {
            unsigned char* rect = static_cast<unsigned char*>(lockedRect.pBits);
            unsigned char* data = static_cast<unsigned char*>(data_);
            
            // Must copy pixels row by row in rect
            size_t rect_idx = 0, rect_offset = 0;
            size_t data_idx = 0;
            size_t size = width_ * height_ * bpp;

            for (size_t j = 0; j < height_; j++) {
                rect_offset = (height_ - j - 1) * lockedRect.Pitch;

                for (size_t i = 0; i < width_; i++) {
                    rect_idx = rect_offset + i * 4;
                    //rect_idx = i * lockedRect.Pitch + rect_offset;
                    //data_idx = (i * width_ + (width_ - j - 1)) * bpp;
                    //data_idx = ((height_ - i - 1) * width_ + j) * bpp;
                    //data_idx = (j * width_ + i) * bpp;

                    rect[rect_idx    ] = data[data_idx    ];
                    rect[rect_idx + 1] = data[data_idx + 1];
                    rect[rect_idx + 2] = data[data_idx + 2];
                    data_idx += bpp;
                    //rect_idx += lockedRect.Pitch;
                }
            }

            /*
            for (size_t i = width_ / 2 - 32; i < width_ / 2 + 32; i++) {
                for (size_t j = height_ / 2 - 32; j < height_ / 2 + 32; j++) {
                    rect_idx = (i * height_ + j) * 4;
                    rect[rect_idx] = 0;
                    rect[rect_idx+1] = 0;
                    rect[rect_idx+2] = 255;
                }
            }
            */
            /*
            for (size_t i = 0; i < width_; i++) {
                for (size_t j = 0; j < height_; j++) {
                    //rect_idx = j * lockedRect.Pitch + i * 4;
                    rect_idx = j * lockedRect.Pitch + i * 4;
                    data_idx = (i * height_ + j) * bpp;

                    rect[rect_idx    ] = data[data_idx + 2];
                    rect[rect_idx + 1] = data[data_idx + 1];
                    rect[rect_idx + 2] = data[data_idx    ];

                    if (bpp == 4) {
                        rect[rect_idx + 3] = data[data_idx + 3];
                    }
                }
            }*/
            
            /*
            for (size_t i = 0; i < height_; i++) {
                for (size_t j = 0; j < width_; j++) {
                    rect_idx = j + lockedRect.Pitch * i;
                    data_idx = (i * width_ + j) * bpp;

                    rect[rect_idx    ] = data[data_idx + 2];
                    rect[rect_idx + 1] = data[data_idx + 1];
                    rect[rect_idx + 2] = data[data_idx    ];

                    if (bpp == 4) {
                        rect[rect_idx + 3] = data[data_idx + 3];
                    } else {
                        rect[rect_idx + 3] = 255;
                    }
                }
            }
            */
            texture_->UnlockRect(0);
        }
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