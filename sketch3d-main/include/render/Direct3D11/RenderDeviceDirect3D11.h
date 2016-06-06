#ifndef SKETCH_3D_RENDER_DEVICE_DIRECT3D11_H
#define SKETCH_3D_RENDER_DEVICE_DIRECT3D11_H

#include "render/RenderDevice.h"

#include "render/RenderState.h"
#include "render/VertexFormat.h"

#include "render/Direct3D11/HardwareResourceCreatorDirect3D11.h"

#include <vector>
using namespace std;

#pragma warning( disable : 4005 )

#include <d3d11.h>

#pragma warning( default : 4005 )

namespace Sketch3D {

// Forward declaration
class ConstantBuffer;
class DepthStencilTarget;
class HardwareResource;
class IndexBuffer;
class FragmentShader;
class RenderContext;
class RenderTarget;
class SamplerState;
class Texture;
class VertexBuffer;
class VertexShader;

/**
 * @class RenderDeviceDirect3D11
 * Direct3D11 implementation of the RenderDevice
 */
class SKETCH_3D_API RenderDeviceDirect3D11 : public RenderDevice {
    public:
                                            RenderDeviceDirect3D11();
        virtual                            ~RenderDeviceDirect3D11();

        virtual bool                        Initialize(const shared_ptr<RenderContext>& renderContext) override;

		virtual void                        ClearRenderTargets(const Vector4& color) override;
        virtual void                        ClearDepthStencil(bool clearDepth, bool clearStencil, float depthValue, unsigned char stencilValue) override;
		virtual void						ClearDepthBuffer(float depthValue=1.0f) override;
        virtual void                        SetRenderTargetsAndDepthStencilTarget(const vector<shared_ptr<RenderTarget>>& renderTargets, const shared_ptr<DepthStencilTarget>& depthStencilTarget) override;
        virtual void                        SetRenderTargets(const vector<shared_ptr<RenderTarget>>& renderTargets) override;
        virtual void                        SetDepthStencilTarget(const shared_ptr<DepthStencilTarget>& depthStencilTarget) override;
        virtual void                        SetDefaultRenderTargetAndDepthStencilBuffer() override;
        
        virtual void                        SetDepthStencilState(const DepthStencilState_t& depthStencilState, unsigned int referenceMask) override;
        virtual void                        SetRasterizerState(const RasterizerState_t& rasterizerState) override;
        virtual void                        SetDefaultDepthStencilState(unsigned int referenceMask) override;
        virtual void                        SetDefaultRasterizerState() override;

        virtual shared_ptr<FragmentShader>& GetFragmentShader() override;
        virtual void                        SetFragmentShader(shared_ptr<FragmentShader> fragmentShader) override;
        virtual bool                        SetFragmentShaderConstantBuffer(const shared_ptr<ConstantBuffer>& constantBuffer, size_t slot) override;
        virtual bool                        SetFragmentShaderSamplerState(const shared_ptr<SamplerState>& samplerState, unsigned int slot) override;
        virtual bool                        SetFragmentShaderTexture(const shared_ptr<Texture1D>& texture, unsigned int slot) override;
        virtual bool                        SetFragmentShaderTexture(const shared_ptr<Texture2D>& texture, unsigned int slot) override;
        virtual bool                        SetFragmentShaderTexture(const shared_ptr<Texture3D>& texture, unsigned int slot) override;
        virtual bool                        SetFragmentShaderTexture(const shared_ptr<RenderTarget>& texture, unsigned int slot) override;

        virtual shared_ptr<VertexShader>&   GetVertexShader() override;
        virtual void                        SetVertexShader(shared_ptr<VertexShader> vertexShader) override;
        virtual bool                        SetVertexShaderConstantBuffer(const shared_ptr<ConstantBuffer>& constantBuffer, size_t slot) override;
        virtual bool                        SetVertexShaderSamplerState(const shared_ptr<SamplerState>& samplerState, unsigned int slot) override;
        virtual bool                        SetVertexShaderTexture(const shared_ptr<Texture1D>& texture, unsigned int slot) override;
        virtual bool                        SetVertexShaderTexture(const shared_ptr<Texture2D>& texture, unsigned int slot) override;
        virtual bool                        SetVertexShaderTexture(const shared_ptr<Texture3D>& texture, unsigned int slot) override;
        virtual bool                        SetVertexShaderTexture(const shared_ptr<RenderTarget>& texture, unsigned int slot) override;

        virtual bool                        Draw(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer, size_t startVertexLocation) override;
        virtual bool                        DrawIndexed(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer,
                                                        const shared_ptr<IndexBuffer>& indexBuffer, size_t startIndexLocation, int baseVertexLocation) override;
        virtual bool                        DrawInstanced(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer, const vector<Matrix4x4>& modelMatrices) override;
        virtual bool                        DrawIndexedInstanced(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer,
                                                                 const shared_ptr<IndexBuffer>& indexBuffer, const vector<Matrix4x4>& modelMatrices) override;

        virtual void                        GenerateMips(Texture* texture) override;
        virtual void                        CopyResource(const shared_ptr<HardwareResource>& source, const shared_ptr<HardwareResource>& destination) override;
        virtual Matrix4x4                   CalculatePerspectiveProjectionRightHanded(float width, float height, float nearPlane, float farPlane) override;
        virtual Matrix4x4                   CalculatePerspectiveProjectionFOVRightHanded(float fov, float aspectRatio, float nearPlane, float farPlane) override;
        virtual Matrix4x4                   CalculatePerspectiveProjectionLeftHanded(float width, float height, float nearPlane, float farPlane) override;
        virtual Matrix4x4                   CalculatePerspectiveProjectionFOVLeftHanded(float fov, float aspectRatio, float nearPlane, float farPlane) override;
        virtual void                        SetViewport(float width, float height) override;
        virtual void                        RestoreViewportToOriginal() override;

        virtual HardwareResourceCreator*    GetHardwareResourceCreator() const override;

        ID3D11Device*                       GetDevice() const;
        ID3D11DeviceContext*                GetDeviceContext() const;

    private:
        ID3D11Device*                       device_;
        ID3D11DeviceContext*                context_;
        ID3D11RenderTargetView*             defaultBackbuffer_;
        ID3D11DepthStencilView*             defaultDepthStencilBuffer_;
        ID3D11Texture2D*                    depthStencilTexture_;

        // This stuff will move in the render state manager
        vector<ID3D11RenderTargetView*>     currentRenderTargets_;
        ID3D11DepthStencilView*             currentDepthStencilBuffer_;
        ID3D11RasterizerState*              currentRasterizerState_;
        ID3D11DepthStencilState*            currentDepthStencilState_;

        ID3D11PixelShader*                  currentPixelShader_;
        ID3D11VertexShader*                 currentVertexShader_;

        HardwareResourceCreatorDirect3D11*  hardwareResourceCreator_;

        virtual bool                        CreateDefaultDepthStencilState(DepthStencilBits_t depthStencilBits) override;
        virtual bool                        CreateDefaultRasterizerState(const shared_ptr<RenderContext>& renderContext) override;
};

// UTILITY FUNCTIONS
D3D11_COMPARISON_FUNC GetD3DComparisonFunc(ComparisonFunction_t func);
D3D11_FILL_MODE GetD3DFillMode(FillMode_t mode);
D3D11_CULL_MODE GetD3DCullMode(CullingMethod_t method);
D3D11_PRIMITIVE_TOPOLOGY GetD3DPrimitiveTopology(PrimitiveTopology_t primitiveTopology);

bool RegisterVertexFormatType(ID3D11Device* device, VertexFormatType_t vertexFormatType, VertexFormat* vertexFormat);

}

#endif