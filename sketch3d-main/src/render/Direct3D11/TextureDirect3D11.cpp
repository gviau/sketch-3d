#include "render/Direct3D11/TextureDirect3D11.h"

#include "render/TextureMap.h"

#include "system/Logger.h"

namespace Sketch3D {
TextureDirect3D11::TextureDirect3D11(ID3D11Device* device) : device_(device), shaderResourceView_(nullptr) {
}

TextureDirect3D11::~TextureDirect3D11() {
    if (shaderResourceView_ != nullptr) {
        shaderResourceView_->Release();
    }
}

ID3D11ShaderResourceView* TextureDirect3D11::GetShaderResourceView() const {
    return shaderResourceView_;
}

bool TextureDirect3D11::CreateShaderResourceView(ID3D11Resource* resource, TextureFormat_t format, TextureType_t textureType) {
    D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
    resourceViewDesc.Format = GetD3DTextureFormat(format);
    
    switch (textureType) {
        case TextureType_t::TYPE_1D:
            resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
            resourceViewDesc.Texture1D.MipLevels = 1;
            resourceViewDesc.Texture1D.MostDetailedMip = 0;
            break;

        case TextureType_t::TYPE_2D:
            resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            resourceViewDesc.Texture2D.MipLevels = 1;
            resourceViewDesc.Texture2D.MostDetailedMip = 0;
            break;

        case TextureType_t::TYPE_3D:
            resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
            resourceViewDesc.Texture3D.MipLevels = 1;
            resourceViewDesc.Texture3D.MostDetailedMip = 0;
            break;
    }

    HRESULT hr = device_->CreateShaderResourceView(resource, &resourceViewDesc, &shaderResourceView_);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}

Texture1DDirect3D11::Texture1DDirect3D11(ID3D11Device* device) : TextureDirect3D11(device), texture_(nullptr) {
}

Texture1DDirect3D11::~Texture1DDirect3D11() {
    if (texture_ != nullptr) {
        texture_->Release();
    }
}

bool Texture1DDirect3D11::Initialize(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable) {
    return false;
}

ID3D11Texture1D* Texture1DDirect3D11::GetTexture() const {
    return texture_;
}

Texture2DDirect3D11::Texture2DDirect3D11(ID3D11Device* device) : TextureDirect3D11(device), texture_(nullptr) {
}

Texture2DDirect3D11::~Texture2DDirect3D11() {
    if (texture_ != nullptr) {
        texture_->Release();
    }
}

bool Texture2DDirect3D11::Initialize(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable) {
    if (texture_ != nullptr) {
        Logger::GetInstance()->Warning("Texture2D with resource id # " + to_string(resourceId_) + " already created");
        return false;
    } else if (textureMap == nullptr) {
        Logger::GetInstance()->Error("Invalid texture map passed unpon initialization of Texture2D");
        return false;
    }

    textureMap_ = textureMap;
    textureFormat_ = textureFormat;

    D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
    if (immutable) {
        usage = D3D11_USAGE_IMMUTABLE;
    } else if (dynamic) {
        usage = D3D11_USAGE_DYNAMIC;
    }

    size_t width = textureMap_->GetWidth();
    size_t height = textureMap_->GetHeight();

    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = GetD3DTextureFormat(textureFormat_);
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = usage;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = (dynamic) ? D3D11_CPU_ACCESS_WRITE : 0;
    textureDesc.MiscFlags = 0;

    size_t rowPitch = GetRowPitch(width, textureFormat_);

    D3D11_SUBRESOURCE_DATA initialData;
    initialData.pSysMem = (void*)textureMap_->GetData();
    initialData.SysMemPitch = rowPitch;
    initialData.SysMemSlicePitch = rowPitch * height;

    HRESULT hr = device_->CreateTexture2D(&textureDesc, &initialData, &texture_);
    if (FAILED(hr)) {
        Logger::GetInstance()->Error("Couldn't create 2D texture with ID: " + to_string(resourceId_));
        return false;
    }
    
    if (!CreateShaderResourceView(texture_, textureFormat_, TextureType_t::TYPE_2D)) {
        Logger::GetInstance()->Error("Couldn't create the shader resource view from resource with ID: " + to_string(resourceId_));
        return false;
    }

    return true;
}

ID3D11Texture2D* Texture2DDirect3D11::GetTexture() const {
    return texture_;
}

Texture3DDirect3D11::Texture3DDirect3D11(ID3D11Device* device) : TextureDirect3D11(device), texture_(nullptr) {
}

Texture3DDirect3D11::~Texture3DDirect3D11() {
    if (texture_ != nullptr) {
        texture_->Release();
    }
}

bool Texture3DDirect3D11::Initialize(TextureMap* textureMap, TextureFormat_t textureFormat, bool dynamic, bool immutable) {
    return false;
}

ID3D11Texture3D* Texture3DDirect3D11::GetTexture() const {
    return texture_;
}

DXGI_FORMAT GetD3DTextureFormat(TextureFormat_t textureFormat) {
    DXGI_FORMAT format;
    switch (textureFormat) {
        case TextureFormat_t::GRAYSCALE:    format = DXGI_FORMAT_R8_UNORM; break;
        case TextureFormat_t::RGB24:        format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
        case TextureFormat_t::RGBA32:       format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
        case TextureFormat_t::R32F:         format = DXGI_FORMAT_R32_FLOAT; break;
        case TextureFormat_t::RG32F:        format = DXGI_FORMAT_R32G32_FLOAT; break;
        case TextureFormat_t::RGBA32F:      format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
        case TextureFormat_t::R16F:         format = DXGI_FORMAT_R16_FLOAT; break;
        case TextureFormat_t::RG16F:        format = DXGI_FORMAT_R16G16_FLOAT; break;
        case TextureFormat_t::RGBA16F:      format = DXGI_FORMAT_R16G16B16A16_FLOAT; break;
        case TextureFormat_t::DEPTH:        format = DXGI_FORMAT_D32_FLOAT; break;
    }

    return format;
}

}