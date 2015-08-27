#include "render/OpenGL/RenderDeviceOpenGL.h"

#include "math/Vector4.h"

#include "render/OpenGL/RenderContextOpenGL.h"

#include "render/OpenGL/GL/gl.h"

namespace Sketch3D {

RenderDeviceOpenGL::RenderDeviceOpenGL()
    : hardwareResourceCreator_(nullptr)
{
}

RenderDeviceOpenGL::~RenderDeviceOpenGL()
{
    delete hardwareResourceCreator_;
}

bool RenderDeviceOpenGL::Initialize(const shared_ptr<RenderContext>& renderContext)
{
    hardwareResourceCreator_ = new HardwareResourceCreatorOpenGL;

    width_ = renderContext->GetRenderParameters().width;
    height_ = renderContext->GetRenderParameters().height;

    CreateDefaultDepthStencilState(renderContext->GetRenderParameters().depthStencilBits);
    CreateDefaultRasterizerState(renderContext);

    return true;
}

void RenderDeviceOpenGL::ClearRenderTargets(const Vector4& color)
{
    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderDeviceOpenGL::ClearDepthStencil(bool clearDepth, bool clearStencil, float depthValue, unsigned char stencilValue)
{
    int clearFlags = ( ( (clearDepth) ? GL_DEPTH_BUFFER_BIT : 0 ) | ( (clearStencil) ? GL_STENCIL_BUFFER_BIT : 0 ) );

    if (clearDepth)
    {
        glClearDepth(depthValue);
    }

    if (clearStencil)
    {
        glClearStencil(stencilValue);
    }

    glClear(clearFlags);
}

void RenderDeviceOpenGL::SetRenderTargets(const vector<shared_ptr<RenderTarget>>& renderTargets, const shared_ptr<DepthStencilTarget>& depthStencilTarget)
{
}

void RenderDeviceOpenGL::SetDepthStencilTarget(const shared_ptr<DepthStencilTarget>& depthStencilTarget)
{
}

void RenderDeviceOpenGL::SetDefaultRenderTargetAndDepthStencilBuffer()
{
}

void RenderDeviceOpenGL::SetDepthStencilState(const DepthStencilState_t& depthStencilState, unsigned int referenceMask)
{
}

void RenderDeviceOpenGL::SetRasterizerState(const RasterizerState_t& rasterizerState)
{
}

void RenderDeviceOpenGL::SetDefaultDepthStencilState(unsigned int referenceMask)
{
}

void RenderDeviceOpenGL::SetDefaultRasterizerState()
{
}

shared_ptr<FragmentShader>& RenderDeviceOpenGL::GetFragmentShader()
{
    return fragmentShader_;
}

void RenderDeviceOpenGL::SetFragmentShader(shared_ptr<FragmentShader> fragmentShader)
{
}

bool RenderDeviceOpenGL::SetFragmentShaderConstantBuffer(const shared_ptr<ConstantBuffer>& constantBuffer, size_t slot)
{
    return false;
}

bool RenderDeviceOpenGL::SetFragmentShaderSamplerState(const shared_ptr<SamplerState>& samplerState, size_t slot)
{
    return false;
}

bool RenderDeviceOpenGL::SetFragmentShaderTexture(const shared_ptr<Texture1D>& texture, unsigned int slot)
{
    return false;
}

bool RenderDeviceOpenGL::SetFragmentShaderTexture(const shared_ptr<Texture2D>& texture, unsigned int slot)
{
    return false;
}

bool RenderDeviceOpenGL::SetFragmentShaderTexture(const shared_ptr<Texture3D>& texture, unsigned int slot)
{
    return false;
}

bool RenderDeviceOpenGL::SetFragmentShaderTexture(const shared_ptr<RenderTarget>& texture, unsigned int slot)
{
    return false;
}

shared_ptr<VertexShader>& RenderDeviceOpenGL::GetVertexShader()
{
    return vertexShader_;
}

void RenderDeviceOpenGL::SetVertexShader(shared_ptr<VertexShader> vertexShader)
{
}

bool RenderDeviceOpenGL::SetVertexShaderConstantBuffer(const shared_ptr<ConstantBuffer>& constantBuffer, size_t slot)
{
    return false;
}

bool RenderDeviceOpenGL::SetVertexShaderSamplerState(const shared_ptr<SamplerState>& samplerState, size_t slot)
{
    return false;
}

bool RenderDeviceOpenGL::SetVertexShaderTexture(const shared_ptr<Texture1D>& texture, unsigned int slot)
{
    return false;
}

bool RenderDeviceOpenGL::SetVertexShaderTexture(const shared_ptr<Texture2D>& texture, unsigned int slot)
{
    return false;
}

bool RenderDeviceOpenGL::SetVertexShaderTexture(const shared_ptr<Texture3D>& texture, unsigned int slot)
{
    return false;
}

bool RenderDeviceOpenGL::SetVertexShaderTexture(const shared_ptr<RenderTarget>& texture, unsigned int slot)
{
    return false;
}

bool RenderDeviceOpenGL::Draw(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer, size_t startVertexLocation)
{
    return false;
}

bool RenderDeviceOpenGL::DrawIndexed(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer,
                                     const shared_ptr<IndexBuffer>& indexBuffer, size_t startIndexLocation, int baseVertexLocation)
{
    return false;
}

bool RenderDeviceOpenGL::DrawInstanced(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer, const vector<Matrix4x4>& modelMatrices)
{
    return false;
}

bool RenderDeviceOpenGL::DrawIndexedInstanced(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer,
                                              const shared_ptr<IndexBuffer>& indexBuffer, const vector<Matrix4x4>& modelMatrices)
{
    return false;
}

void RenderDeviceOpenGL::GenerateMips(Texture* texture)
{
}

void RenderDeviceOpenGL::CopyResource(const shared_ptr<HardwareResource>& source, const shared_ptr<HardwareResource>& destination)
{
}

HardwareResourceCreator* RenderDeviceOpenGL::GetHardwareResourceCreator() const
{
    return hardwareResourceCreator_;
}

bool RenderDeviceOpenGL::CreateDefaultDepthStencilState(DepthStencilBits_t depthStencilBits)
{
    return false;
}

bool RenderDeviceOpenGL::CreateDefaultRasterizerState(const shared_ptr<RenderContext>& renderContext)
{
    return false;
}

}