#ifndef SKETCH_3D_RENDER_TARGET_DIRECT3D11_H
#define SKETCH_3D_RENDER_TARGET_DIRECT3D11_H

#include "render/RenderTarget.h"

#pragma warning( disable : 4005 )

#include <d3d11.h>

#pragma warning( default : 4005 )

namespace Sketch3D {
class SKETCH_3D_API RenderViewDirect3D11 {
    public:
                                    RenderViewDirect3D11(ID3D11Device* device);
        virtual                    ~RenderViewDirect3D11();

        ID3D11ShaderResourceView*   GetResourceView() const;

    protected:
        ID3D11Device*               device_;
        ID3D11ShaderResourceView*   resourceView_;
};

class SKETCH_3D_API RenderTargetDirect3D11 : public RenderTarget, public RenderViewDirect3D11 {
    public:
                                RenderTargetDirect3D11(ID3D11Device* device);
        virtual                ~RenderTargetDirect3D11();

        virtual bool            Initialize(size_t width, size_t height, TextureFormat_t textureFormat) override;
        ID3D11RenderTargetView* GetRenderTargetView() const;

    private:
        ID3D11RenderTargetView* renderTargetView_;
};

class SKETCH_3D_API DepthStencilTargetDirect3D11 : public DepthStencilTarget, public RenderViewDirect3D11 {
    public:
                                DepthStencilTargetDirect3D11(ID3D11Device* device);
        virtual                ~DepthStencilTargetDirect3D11();

        virtual bool            Initialize(size_t width, size_t height, DepthStencilBits_t depthStencilBits) override;
        ID3D11DepthStencilView* GetDepthStencilView() const;

    private:
        ID3D11DepthStencilView* depthStencilView_;
};

DXGI_FORMAT GetD3DRenderTargetFormat(TextureFormat_t textureFormat);
DXGI_FORMAT GetD3DDepthStencilTextureFormat(DepthStencilBits_t depthStencilBits);
DXGI_FORMAT GetD3DDepthStencilViewFormat(DepthStencilBits_t depthStencilBits);
DXGI_FORMAT GetD3DDepthStencilTargetFormat(DepthStencilBits_t depthStencilBits);
}

#endif