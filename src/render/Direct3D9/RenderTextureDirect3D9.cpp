#include "render/Direct3D9/RenderTextureDirect3D9.h"

#include "render/Direct3D9/Texture2DDirect3D9.h"

#include "system/Logger.h"

#include <d3d9.h>
#include <d3dx9.h>

namespace Sketch3D {
RenderTextureDirect3D9::RenderTextureDirect3D9(IDirect3DDevice9* device, unsigned int width, unsigned int height, TextureFormat_t format) :
    RenderTexture(width, height, format), device_(device), depthBufferSurface_(nullptr)
{
}

RenderTextureDirect3D9::~RenderTextureDirect3D9() {
    if (depthBufferSurface_ != nullptr) {
        depthBufferSurface_->Release();
    }

    for (size_t i = 0; i < attachedSurfaces_.size(); i++) {
        if (attachedSurfaces_[i] != nullptr) {
            attachedSurfaces_[i]->Release();
        }
    }
}

bool RenderTextureDirect3D9::AddDepthBuffer() {
    if (depthBufferBound_) {
        return false;
    } else if (depthBufferSurface_ != nullptr) {
        return true;
    }

    HRESULT hr = device_->CreateDepthStencilSurface(width_, height_, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, FALSE, &depthBufferSurface_, nullptr);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}

Texture2D* RenderTextureDirect3D9::CreateTexture2D() const {
    // We need to manually create the texture to set the usage flag to D3DUSAGE_RENDERTARGET
    Texture2DDirect3D9* texture = new Texture2DDirect3D9(device_);

    D3DFORMAT format;
    switch (format_) {
        case TEXTURE_FORMAT_RGB24:
            format = D3DFMT_R8G8B8;
            break;

        case TEXTURE_FORMAT_RGBA32:
            format = D3DFMT_A8R8G8B8;
            break;

        case TEXTURE_FORMAT_R32F:
            format = D3DFMT_R32F;
            break;

        case TEXTURE_FORMAT_RG32F:
            format = D3DFMT_G32R32F;
            break;

        case TEXTURE_FORMAT_RGBA32F:
            format = D3DFMT_A32B32G32R32F;
            break;

        default:
            Logger::GetInstance()->Warning("Unsupported texture format");
            return false;
    }

    D3DXCreateTexture(device_, width_, height_, 1, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, &texture->texture_);

    if (texture->texture_ == nullptr) {
        Logger::GetInstance()->Error("Couldn't create texture");
        delete texture;
        return nullptr;
    }

    texture->width_ = width_;
    texture->height_ = height_;
    texture->format_ = format_;

    return texture;
}

Texture2D* RenderTextureDirect3D9::CreateDepthBufferTexture() const {
    // We need to manually create the texture to set the usage flag to D3DUSAGE_RENDERTARGET
    Texture2DDirect3D9* texture = new Texture2DDirect3D9(device_);

    D3DXCreateTexture(device_, width_, height_, 1, D3DUSAGE_DEPTHSTENCIL, D3DFMT_D24X8, D3DPOOL_DEFAULT, &texture->texture_);

    if (texture->texture_ == nullptr) {
        Logger::GetInstance()->Error("Couldn't create texture");
        delete texture;
        return nullptr;
    }

    texture->width_ = width_;
    texture->height_ = height_;
    texture->format_ = TEXTURE_FORMAT_DEPTH;

    return texture;
}

bool RenderTextureDirect3D9::AttachTextureToDepthBuffer(Texture2D* texture) {
    if (depthBufferBound_) {
        return true;
    } else if (depthBufferSurface_ != nullptr) {
        return false;
    }

    if (texture->GetWidth() != width_ || texture->GetHeight() != height_) {
        Logger::GetInstance()->Error("Texture used as attachment for the depth buffer isn't of the same size as the render texture");
        return false;
    }

    Texture2DDirect3D9* d3dTexture = static_cast<Texture2DDirect3D9*>(texture);

    IDirect3DSurface9* textureSurface = nullptr;
    d3dTexture->texture_->GetSurfaceLevel(0, &textureSurface);
    if (textureSurface == nullptr) {
        return false;
    }

    depthBufferSurface_ = textureSurface;
    depthBufferBound_ = true;
    Logger::GetInstance()->Info("Texture successfully attached to depth buffer");

    return true;
}

bool RenderTextureDirect3D9::AttachTextures(const vector<Texture2D*>& textures) {
    if (texturesAttached_) {
        return true;
    }

    for (size_t i = 0; i < textures.size(); i++) {
        Texture2D* texture = textures[i];

        if (texture != nullptr) {
            if (texture->GetWidth() != width_) {
                Logger::GetInstance()->Error("Couldn't create render texture, texture #" + to_string(i) + " doesn't have the appropriate width.");
                return false;
            }

            if (texture->GetHeight() != height_) {
                Logger::GetInstance()->Error("Couldn't create render texture, texture #" + to_string(i) + " doesn't have the appropriate height.");
                return false;
            }

            if (texture->GetTextureFormat() != format_) {
                Logger::GetInstance()->Error("Couldn't create render texture, texture #" + to_string(i) + " doesn't have the appropriate format.");
                return false;
            }

        } else {
            Logger::GetInstance()->Error("Couldn't create render texture, texture #" + to_string(i) + " is null.");
            return false;
        }

        Texture2DDirect3D9* d3dTexture = static_cast<Texture2DDirect3D9*>(textures[i]);

        IDirect3DSurface9* textureSurface = nullptr;
        d3dTexture->texture_->GetSurfaceLevel(0, &textureSurface);
        if (textureSurface == nullptr) {
            return false;
        }

        attachedSurfaces_.push_back(textureSurface);
    }

    texturesAttached_ = true;
    Logger::GetInstance()->Info("Render texture was succesfully created");

    return true;
}

void RenderTextureDirect3D9::Bind() const {
    if (depthBufferSurface_ != nullptr) {
        device_->SetDepthStencilSurface(depthBufferSurface_);
    }

    for (size_t i = 0; i < attachedSurfaces_.size(); i++) {
        device_->SetRenderTarget(i, attachedSurfaces_[i]);
    }
}

}