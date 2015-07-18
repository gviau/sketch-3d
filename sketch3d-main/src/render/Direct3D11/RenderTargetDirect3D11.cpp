#include "render/Direct3D11/RenderTargetDirect3D11.h"

#include "system/Logger.h"

namespace Sketch3D {
RenderViewDirect3D11::RenderViewDirect3D11(ID3D11Device* device) : resourceView_(nullptr) {
}

RenderViewDirect3D11::~RenderViewDirect3D11() {
    if (resourceView_ != nullptr) {
        resourceView_->Release();
    }
}

ID3D11ShaderResourceView* RenderViewDirect3D11::GetResourceView() const {
    return resourceView_;
}

RenderTargetDirect3D11::RenderTargetDirect3D11(ID3D11Device* device, size_t width, size_t height, TextureFormat_t format) :
        RenderViewDirect3D11(device), RenderTarget(width, height, format), renderTargetView_(nullptr)
{
    ID3D11Texture2D* texture;
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width = width_;
    textureDesc.Height = height_;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = GetD3DRenderTargetFormat(format_);
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, &texture);
    if (FAILED(hr)) {
        Logger::GetInstance()->Error("Couldn't create render target texture");
    } else {

        D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;
        renderTargetDesc.Format = textureDesc.Format;
        renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetDesc.Texture2D.MipSlice = 0;

        hr = device->CreateRenderTargetView(texture, &renderTargetDesc, &renderTargetView_);
        if (FAILED(hr)) {
            Logger::GetInstance()->Error("Couldn't create render target");
            texture->Release();
        } else {

            D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
            resourceViewDesc.Format = textureDesc.Format;
            resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            resourceViewDesc.Texture2D.MipLevels = 1;
            resourceViewDesc.Texture2D.MostDetailedMip = 0;

            hr = device->CreateShaderResourceView(texture, &resourceViewDesc, &resourceView_);
            if (FAILED(hr)) {
                Logger::GetInstance()->Error("Couldn't create shader resource view for render target");
            }

            texture->Release();
        }
    }
}

RenderTargetDirect3D11::~RenderTargetDirect3D11() {
    if (renderTargetView_ != nullptr) {
        renderTargetView_->Release();
    }
}

ID3D11RenderTargetView* RenderTargetDirect3D11::GetRenderTargetView() const {
    return renderTargetView_;
}

DepthStencilTargetDirect3D11::DepthStencilTargetDirect3D11(ID3D11Device* device, size_t width, size_t height, DepthStencilBits_t depthStencilBits) :
        RenderViewDirect3D11(device), DepthStencilTarget(width, height), depthStencilView_(nullptr)
{
    ID3D11Texture2D* texture;
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width = width_;
    textureDesc.Height = height_;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = GetD3DDepthStencilTextureFormat(depthStencilBits);
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    
    HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, &texture);
    if (FAILED(hr)) {
        Logger::GetInstance()->Error("Couldn't create depth stencil target texture");
    } else{

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        depthStencilViewDesc.Format = GetD3DDepthStencilViewFormat(depthStencilBits);
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;
        depthStencilViewDesc.Flags = 0;

        hr = device->CreateDepthStencilView(texture, &depthStencilViewDesc, &depthStencilView_);
        if (FAILED(hr)) {
            Logger::GetInstance()->Error("Couldn't create depth stencil target");
            texture->Release();
        } else {

            D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
            resourceViewDesc.Format = GetD3DDepthStencilTargetFormat(depthStencilBits);
            resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            resourceViewDesc.Texture2D.MipLevels = 1;
            resourceViewDesc.Texture2D.MostDetailedMip = 0;

            hr = device->CreateShaderResourceView(texture, &resourceViewDesc, &resourceView_);
            if (FAILED(hr)) {
                Logger::GetInstance()->Error("Couldn't create shader resource view for depth stencil target");
            }

            texture->Release();
        }
    }
}

DepthStencilTargetDirect3D11::~DepthStencilTargetDirect3D11() {
    if (depthStencilView_ != nullptr) {
        depthStencilView_->Release();
    }
}

ID3D11DepthStencilView* DepthStencilTargetDirect3D11::GetDepthStencilView() const {
    return depthStencilView_;
}

DXGI_FORMAT GetD3DRenderTargetFormat(TextureFormat_t textureFormat) {
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
    }

    return format;
}

DXGI_FORMAT GetD3DDepthStencilTextureFormat(DepthStencilBits_t depthStencilBits) {
    DXGI_FORMAT format;
    switch (depthStencilBits) {
        case DepthStencilBits_t::D16:   format = DXGI_FORMAT_R16_TYPELESS; break;
        case DepthStencilBits_t::D24S8: format = DXGI_FORMAT_R24G8_TYPELESS; break;
        case DepthStencilBits_t::D32:   format = DXGI_FORMAT_R32_TYPELESS; break;
    }

    return format;
}

DXGI_FORMAT GetD3DDepthStencilViewFormat(DepthStencilBits_t depthStencilBits) {
    DXGI_FORMAT format;
    switch (depthStencilBits) {
        case DepthStencilBits_t::D16:   format = DXGI_FORMAT_R16_UNORM; break;
        case DepthStencilBits_t::D24S8: format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;
        case DepthStencilBits_t::D32:   format = DXGI_FORMAT_D32_FLOAT; break;
    }

    return format;
}

DXGI_FORMAT GetD3DDepthStencilTargetFormat(DepthStencilBits_t depthStencilBits) {
    DXGI_FORMAT format;
    switch (depthStencilBits) {
        case DepthStencilBits_t::D16:   format = DXGI_FORMAT_R16_UNORM; break;
        case DepthStencilBits_t::D24S8: format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;
        case DepthStencilBits_t::D32:   format = DXGI_FORMAT_R32_FLOAT; break;
    }

    return format;
}

}