#ifndef SKETCH_3D_HARDWARE_RESOURCE_CREATOR_DIRECT3D11_H
#define SKETCH_3D_HARDWARE_RESOURCE_CREATOR_DIRECT3D11_H

#include "render/HardwareResourceCreator.h"

// Forward struct declaration
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace Sketch3D {
/**
 * @class HardwareResourceCreatorDirect3D11
 * Direct3D11 implementation of the hardware resource creator
 */
class SKETCH_3D_API HardwareResourceCreatorDirect3D11 : public HardwareResourceCreator {
    public:
                                                HardwareResourceCreatorDirect3D11(ID3D11Device* device, ID3D11DeviceContext* context);

        virtual shared_ptr<ConstantBuffer>      CreateConstantBuffer() override;
        virtual shared_ptr<DepthStencilTarget>  CreateDepthStencilTarget() override;
        virtual shared_ptr<IndexBuffer>         CreateIndexBuffer() override;
        virtual shared_ptr<FragmentShader>      CreateFragmentShader() override;
        virtual shared_ptr<RenderTarget>        CreateRenderTarget() override;
        virtual shared_ptr<SamplerState>        CreateSamplerState() override;
        virtual shared_ptr<Texture2D>           CreateTexture2D() override;
        virtual shared_ptr<VertexBuffer>        CreateVertexBuffer() override;
        virtual shared_ptr<VertexShader>        CreateVertexShader() override;

    private:
        ID3D11Device*                       device_;
        ID3D11DeviceContext*                context_;
};
}

#endif