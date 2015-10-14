#ifndef SKETCH_3D_RENDER_DEVICE_H
#define SKETCH_3D_RENDER_DEVICE_H

#include "system/Platform.h"

#include "math/Matrix4x4.h"

#include "render/RenderDevice_Common.h"
#include "render/Renderer_Common.h"

#include <memory>
#include <vector>
using namespace std;

namespace Sketch3D {

// Forward declaration
class Buffer;
class ConstantBuffer;
class DepthStencilTarget;
class HardwareResource;
class HardwareResourceCreator;
class IndexBuffer;
class FragmentShader;
class RenderContext;
class RenderTarget;
class SamplerState;
class Texture;
class Texture1D;
class Texture2D;
class Texture3D;
class VertexBuffer;
class VertexFormat;
class VertexShader;

/**
 * @class RenderDevice
 * This class represents the base class for the APIs' implementation of a render device. The RenderDevice
 * is responsible to draw meshes, set shaders and sampler states.
 */
class SKETCH_3D_API RenderDevice {
    public:
        virtual    ~RenderDevice();

        virtual bool Initialize(const shared_ptr<RenderContext>& renderContext) = 0;

        // RENDER TARGET STUFF
		virtual void ClearRenderTargets(const Vector4& color) = 0;
        virtual void ClearDepthStencil(bool clearDepth, bool clearStencil, float depthValue, unsigned char stencilValue) = 0;
        virtual void SetRenderTargetsAndDepthStencilTarget(const vector<shared_ptr<RenderTarget>>& renderTargets, const shared_ptr<DepthStencilTarget>& depthStencilTarget) = 0;
        virtual void SetRenderTargets(const vector<shared_ptr<RenderTarget>>& renderTargets) = 0;
        virtual void SetDepthStencilTarget(const shared_ptr<DepthStencilTarget>& depthStencilTarget) = 0;
        virtual void SetDefaultRenderTargetAndDepthStencilBuffer() = 0;
        virtual void SetDefaultRenderTarget() = 0;
        virtual void SetDefaultDepthStencilTarget() = 0;

        // RENDER STATE STUFF
        virtual void SetDepthStencilState(const DepthStencilState_t& depthStencilState, unsigned int referenceMask) = 0;
        virtual void SetRasterizerState(const RasterizerState_t& rasterizerState) = 0;
        virtual void SetDefaultDepthStencilState(unsigned int referenceMask) = 0;
        virtual void SetDefaultRasterizerState() = 0;

        // SHADERS STUFF
        virtual shared_ptr<FragmentShader>& GetFragmentShader() = 0;
        virtual void SetFragmentShader(shared_ptr<FragmentShader> fragmentShader) = 0;
        virtual bool SetFragmentShaderConstantBuffer(const shared_ptr<ConstantBuffer>& constantBuffer, size_t slot) = 0;
        virtual bool SetFragmentShaderSamplerState(const shared_ptr<SamplerState>& samplerState, unsigned int slot) = 0;
        virtual bool SetFragmentShaderTexture(const shared_ptr<Texture1D>& texture, unsigned int slot) = 0;
        virtual bool SetFragmentShaderTexture(const shared_ptr<Texture2D>& texture, unsigned int slot) = 0;
        virtual bool SetFragmentShaderTexture(const shared_ptr<Texture3D>& texture, unsigned int slot) = 0;
        virtual bool SetFragmentShaderTexture(const shared_ptr<RenderTarget>& texture, unsigned int slot) = 0;

        virtual shared_ptr<VertexShader>& GetVertexShader() = 0;
        virtual void SetVertexShader(shared_ptr<VertexShader> vertexShader) = 0;
        virtual bool SetVertexShaderConstantBuffer(const shared_ptr<ConstantBuffer>& constantBuffer, size_t slot) = 0;
        virtual bool SetVertexShaderSamplerState(const shared_ptr<SamplerState>& samplerState, unsigned int slot) = 0;
        virtual bool SetVertexShaderTexture(const shared_ptr<Texture1D>& texture, unsigned int slot) = 0;
        virtual bool SetVertexShaderTexture(const shared_ptr<Texture2D>& texture, unsigned int slot) = 0;
        virtual bool SetVertexShaderTexture(const shared_ptr<Texture3D>& texture, unsigned int slot) = 0;
        virtual bool SetVertexShaderTexture(const shared_ptr<RenderTarget>& texture, unsigned int slot) = 0;

        // DRAWING STUFF
        virtual bool Draw(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer, size_t startVertexLocation) = 0;
        virtual bool DrawIndexed(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer,
                                 const shared_ptr<IndexBuffer>& indexBuffer, size_t startIndexLocation, int baseVertexLocation) = 0;
        virtual bool DrawInstanced(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer, const vector<Matrix4x4>& modelMatrices) = 0;
        virtual bool DrawIndexedInstanced(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer,
                                          const shared_ptr<IndexBuffer>& indexBuffer, const vector<Matrix4x4>& modelMatrices) = 0;

        // MISC STUFF
        virtual void GenerateMips(Texture* texture) = 0;
        virtual void CopyResource(const shared_ptr<HardwareResource>& source, const shared_ptr<HardwareResource>& destination) = 0;
        virtual Matrix4x4 CalculatePerspectiveProjection(float width, float height, float nearPlane, float farPlane) = 0;
        virtual Matrix4x4 CalculatePerspectiveProjectionFOV(float fov, float aspectRatio, float nearPlane, float farPlane) = 0;
        virtual void SetViewport(float width, float height) = 0;
        virtual void RestoreViewportToOriginal() = 0;
        
        virtual HardwareResourceCreator* GetHardwareResourceCreator() const = 0;
        const DepthStencilState_t& GetDefaultDepthStencilState() const { return defaultDepthStencilState_; }
        DepthStencilState_t& GetDefaultDepthStencilState() { return defaultDepthStencilState_; }
        const RasterizerState_t& GetDefaultRasterizerState() const { return defaultRasterizerState_; }
        RasterizerState_t& GetDefaultRasterizerState() { return defaultRasterizerState_; }
        const Matrix4x4& GetProjectionMatrix() const { return projection_; }

    protected:
        shared_ptr<FragmentShader>  fragmentShader_;
        shared_ptr<VertexShader>    vertexShader_;
        DepthStencilState_t         defaultDepthStencilState_;
        RasterizerState_t           defaultRasterizerState_;
        size_t                      width_;
        size_t                      height_;

        Matrix4x4                   projection_;

        virtual bool CreateDefaultDepthStencilState(DepthStencilBits_t depthStencilBits) = 0;
        virtual bool CreateDefaultRasterizerState(const shared_ptr<RenderContext>& renderContext) = 0;
};

shared_ptr<RenderDevice> SKETCH_3D_API CreateRenderDevice(RenderSystem_t renderSystem);

}

#endif