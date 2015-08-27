#ifndef SKETCH_3D_RENDER_DEVICE_OPENGL_H
#define SKETCH_3D_RENDER_DEVICE_OPENGL_H

#include "render/RenderDevice.h"

#include "render/RenderState.h"

#include "render/OpenGL/HardwareResourceCreatorOpenGL.h"

#include <vector>
using namespace std;



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
class VertexFormat;
class VertexShader;

/**
 * @class RenderDeviceOpenGL
 * OpenGL implementation of the RenderDevice
 */
class SKETCH_3D_API RenderDeviceOpenGL : public RenderDevice {
    public:
                                            RenderDeviceOpenGL();
        virtual                            ~RenderDeviceOpenGL();

        virtual bool                        Initialize(const shared_ptr<RenderContext>& renderContext) override;

		virtual void                        ClearRenderTargets(const Vector4& color) override;
        virtual void                        ClearDepthStencil(bool clearDepth, bool clearStencil, float depthValue, unsigned char stencilValue) override;
        virtual void                        SetRenderTargets(const vector<shared_ptr<RenderTarget>>& renderTargets, const shared_ptr<DepthStencilTarget>& depthStencilTarget) override;
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

        virtual HardwareResourceCreator*    GetHardwareResourceCreator() const override;

    private:
        HardwareResourceCreatorOpenGL*      hardwareResourceCreator_;

        virtual bool                        CreateDefaultDepthStencilState(DepthStencilBits_t depthStencilBits) override;
        virtual bool                        CreateDefaultRasterizerState(const shared_ptr<RenderContext>& renderContext) override;
};

}

#endif