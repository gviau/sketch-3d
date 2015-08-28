#include "render/OpenGL/RenderDeviceOpenGL.h"

#include "math/Vector4.h"

#include "render/OpenGL/BufferOpenGL.h"
#include "render/OpenGL/RenderContextOpenGL.h"
#include "render/OpenGL/ShaderOpenGL.h"

namespace Sketch3D {

RenderDeviceOpenGL::RenderDeviceOpenGL()
    : pipeline_(0)
    , vertexArrayObject_(0)
    , hardwareResourceCreator_(nullptr)
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

    glGenProgramPipelines(1, &pipeline_);

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
    FragmentShader* fragmentShaderPtr = fragmentShader.get();
    if (fragmentShaderPtr == nullptr)
    {
        return;
    }

    fragmentShader_ = fragmentShader;

    FragmentShaderOpenGL* fragmentShaderOGL = dynamic_cast<FragmentShaderOpenGL*>(fragmentShaderPtr);
    
    glBindProgramPipeline(pipeline_);
    glUseProgramStages(pipeline_, GL_FRAGMENT_SHADER_BIT, fragmentShaderOGL->GetShader());
    glActiveShaderProgram(pipeline_, fragmentShaderOGL->GetShader());
}

bool RenderDeviceOpenGL::SetFragmentShaderConstantBuffer(const shared_ptr<ConstantBuffer>& constantBuffer, size_t slot)
{
    FragmentShader* fragmentShaderPtr = fragmentShader_.get();
    if (fragmentShaderPtr == nullptr)
    {
        return false;
    }

    ConstantBuffer* constantBufferPtr = constantBuffer.get();
    if (constantBufferPtr == nullptr)
    {
        return false;
    }
    
    FragmentShaderOpenGL* fragmentShaderOGL = dynamic_cast<FragmentShaderOpenGL*>(fragmentShaderPtr);
    ConstantBufferOpenGL* constantBufferOGL = dynamic_cast<ConstantBufferOpenGL*>(constantBufferPtr);

    glUniformBlockBinding(fragmentShaderOGL->GetShader(), slot, constantBufferOGL->GetBuffer());

    return true;
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
    VertexShader* vertexShaderPtr = vertexShader.get();
    if (vertexShaderPtr == nullptr)
    {
        return;
    }

    vertexShader_ = vertexShader;

    VertexShaderOpenGL* vertexShaderOGL = dynamic_cast<VertexShaderOpenGL*>(vertexShaderPtr);
    
    glBindProgramPipeline(pipeline_);
    glUseProgramStages(pipeline_, GL_VERTEX_SHADER_BIT, vertexShaderOGL->GetShader());
    glActiveShaderProgram(pipeline_, vertexShaderOGL->GetShader());
}

bool RenderDeviceOpenGL::SetVertexShaderConstantBuffer(const shared_ptr<ConstantBuffer>& constantBuffer, size_t slot)
{
    VertexShader* vertexShaderPtr = vertexShader_.get();
    if (vertexShaderPtr == nullptr)
    {
        return false;
    }

    ConstantBuffer* constantBufferPtr = constantBuffer.get();
    if (constantBufferPtr == nullptr)
    {
        return false;
    }
    
    VertexShaderOpenGL* vertexShaderOGL = dynamic_cast<VertexShaderOpenGL*>(vertexShaderPtr);
    ConstantBufferOpenGL* constantBufferOGL = dynamic_cast<ConstantBufferOpenGL*>(constantBufferPtr);

    glUniformBlockBinding(vertexShaderOGL->GetShader(), slot, constantBufferOGL->GetBuffer());

    return true;
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
    if (vertexShader_ == nullptr)
    {
        return false;
    }

    if (vertexArrayObject_ == 0)
    {
        glGenVertexArrays(1, &vertexArrayObject_);
    }

    VertexShaderOpenGL* vertexShader = dynamic_cast<VertexShaderOpenGL*>(vertexShader_.get());
    VertexFormat* vertexFormat = vertexShader->GetVertexFormat();
    const vector<InputLayout_t>& inputLayouts = vertexFormat->GetInputLayouts();

    glBindVertexArray(vertexArrayObject_);

    VertexBufferOpenGL* vertexBufferOGL = dynamic_cast<VertexBufferOpenGL*>(vertexBuffer.get());
    if (vertexBufferOGL == nullptr)
    {
        return false;
    }

    GLuint buffer = vertexBufferOGL->GetBuffer();
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    
    // This is not very nice :/ have to find a better way
    size_t idx = 0;
    for (InputLayout_t inputLayout : inputLayouts) {
        glEnableVertexArrayAttrib(GL_ARRAY_BUFFER, idx);
        glVertexAttribPointer(idx, GetFormatSize(inputLayout.format), GetOGLFormat(inputLayout.format), GL_FALSE, vertexFormat->GetSize(), (void*)inputLayout.byteOffset);

        idx += 1;
    }

    GLenum topology = GetOGLTopology(primitiveTopology);

    glDrawArrays(topology, startVertexLocation, vertexBuffer->GetNumVertices());

    return true;
}

bool RenderDeviceOpenGL::DrawIndexed(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer,
                                     const shared_ptr<IndexBuffer>& indexBuffer, size_t startIndexLocation, int baseVertexLocation)
{
    if (vertexShader_ == nullptr)
    {
        return false;
    }

    if (vertexArrayObject_ == 0)
    {
        glGenVertexArrays(1, &vertexArrayObject_);
    }

    VertexShaderOpenGL* vertexShader = dynamic_cast<VertexShaderOpenGL*>(vertexShader_.get());
    VertexFormat* vertexFormat = vertexShader->GetVertexFormat();
    const vector<InputLayout_t>& inputLayouts = vertexFormat->GetInputLayouts();

    glBindVertexArray(vertexArrayObject_);

    VertexBufferOpenGL* vertexBufferOGL = dynamic_cast<VertexBufferOpenGL*>(vertexBuffer.get());
    if (vertexBufferOGL == nullptr)
    {
        return false;
    }

    GLuint buffer = vertexBufferOGL->GetBuffer();
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    
    // This is not very nice :/ have to find a better way
    size_t idx = 0;
    for (InputLayout_t inputLayout : inputLayouts) {
        glEnableVertexArrayAttrib(GL_ARRAY_BUFFER, idx);
        glVertexAttribPointer(idx, GetFormatSize(inputLayout.format), GetOGLFormat(inputLayout.format), GL_FALSE, vertexFormat->GetSize(), (void*)inputLayout.byteOffset);

        idx += 1;
    }

    IndexBufferOpenGL* indexBufferOGL = dynamic_cast<IndexBufferOpenGL*>(indexBuffer.get());
    if (indexBufferOGL == nullptr)
    {
        return false;
    }

    buffer = indexBufferOGL->GetBuffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);

    GLenum topology = GetOGLTopology(primitiveTopology);

    GLenum type;
    if (indexBuffer->GetIndexFormat() == IndexFormat_t::INT_2_BYTES)
    {
        type = GL_UNSIGNED_SHORT;
    }
    else
    {
        type = GL_UNSIGNED_INT;
    }

    glDrawElements(topology, indexBuffer->GetNumIndices(), type, nullptr);

    return true;
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

GLint GetFormatSize(InputFormat_t inputFormat)
{
    GLint size = 0;

    switch (inputFormat) {
        case InputFormat_t::BYTE_UINT1:         size = 1; break;
        case InputFormat_t::BYTE_UINT2:         size = 2; break;
        case InputFormat_t::BYTE_UINT4:         size = 4; break;

        case InputFormat_t::BYTE_SINT1:         size = 1; break;
        case InputFormat_t::BYTE_SINT2:         size = 2; break;
        case InputFormat_t::BYTE_SINT4:         size = 4; break;

        case InputFormat_t::HALF_FLOAT1:        size = 1; break;
        case InputFormat_t::HALF_FLOAT2:        size = 2; break;
        case InputFormat_t::HALF_FLOAT4:        size = 4; break;

        case InputFormat_t::HALF_UINT1:         size = 1; break;
        case InputFormat_t::HALF_UINT2:         size = 2; break;
        case InputFormat_t::HALF_UINT4:         size = 4; break;

        case InputFormat_t::HALF_SINT1:         size = 1; break;
        case InputFormat_t::HALF_SINT2:         size = 2; break;
        case InputFormat_t::HALF_SINT4:         size = 4; break;

        case InputFormat_t::FLOAT1:             size = 1; break;
        case InputFormat_t::FLOAT2:             size = 2; break;
        case InputFormat_t::FLOAT3:             size = 3; break;
        case InputFormat_t::FLOAT4:             size = 4; break;

        case InputFormat_t::UINT1:              size = 1; break;
        case InputFormat_t::UINT2:              size = 2; break;
        case InputFormat_t::UINT3:              size = 3; break;
        case InputFormat_t::UINT4:              size = 4; break;

        case InputFormat_t::SINT1:              size = 1; break;
        case InputFormat_t::SINT2:              size = 2; break;
        case InputFormat_t::SINT3:              size = 3; break;
        case InputFormat_t::SINT4:              size = 4; break;

        case InputFormat_t::R10G10B10A2_UINT:   size = 4; break;
        case InputFormat_t::R11G11B10_FLOAT:    size = 4; break;
    }

    return size;
}

GLenum GetOGLFormat(InputFormat_t inputFormat)
{
    GLenum format;

    switch (inputFormat) {
        case InputFormat_t::BYTE_UINT1:         format = GL_UNSIGNED_BYTE; break;
        case InputFormat_t::BYTE_UINT2:         format = GL_UNSIGNED_BYTE; break;
        case InputFormat_t::BYTE_UINT4:         format = GL_UNSIGNED_BYTE; break;

        case InputFormat_t::BYTE_SINT1:         format = GL_BYTE; break;
        case InputFormat_t::BYTE_SINT2:         format = GL_BYTE; break;
        case InputFormat_t::BYTE_SINT4:         format = GL_BYTE; break;

        case InputFormat_t::HALF_FLOAT1:        format = GL_HALF_FLOAT; break;
        case InputFormat_t::HALF_FLOAT2:        format = GL_HALF_FLOAT; break;
        case InputFormat_t::HALF_FLOAT4:        format = GL_HALF_FLOAT; break;

        case InputFormat_t::HALF_UINT1:         format = GL_UNSIGNED_SHORT; break;
        case InputFormat_t::HALF_UINT2:         format = GL_UNSIGNED_SHORT; break;
        case InputFormat_t::HALF_UINT4:         format = GL_UNSIGNED_SHORT; break;

        case InputFormat_t::HALF_SINT1:         format = GL_SHORT; break;
        case InputFormat_t::HALF_SINT2:         format = GL_SHORT; break;
        case InputFormat_t::HALF_SINT4:         format = GL_SHORT; break;

        case InputFormat_t::FLOAT1:             format = GL_FLOAT; break;
        case InputFormat_t::FLOAT2:             format = GL_FLOAT; break;
        case InputFormat_t::FLOAT3:             format = GL_FLOAT; break;
        case InputFormat_t::FLOAT4:             format = GL_FLOAT; break;

        case InputFormat_t::UINT1:              format = GL_UNSIGNED_INT; break;
        case InputFormat_t::UINT2:              format = GL_UNSIGNED_INT; break;
        case InputFormat_t::UINT3:              format = GL_UNSIGNED_INT; break;
        case InputFormat_t::UINT4:              format = GL_UNSIGNED_INT; break;

        case InputFormat_t::SINT1:              format = GL_INT; break;
        case InputFormat_t::SINT2:              format = GL_INT; break;
        case InputFormat_t::SINT3:              format = GL_INT; break;
        case InputFormat_t::SINT4:              format = GL_INT; break;

        case InputFormat_t::R10G10B10A2_UINT:   format = GL_UNSIGNED_INT_10_10_10_2; break;
        case InputFormat_t::R11G11B10_FLOAT:    format = GL_UNSIGNED_INT_10F_11F_11F_REV; break;
    }

    return format;
}

GLenum GetOGLTopology(PrimitiveTopology_t primitiveTopology)
{
    GLenum topology;
    switch (primitiveTopology) {
        case PrimitiveTopology_t::LINELIST:         topology = GL_LINES; break;
        case PrimitiveTopology_t::LINESTRIP:        topology = GL_LINE_STRIP; break;
        case PrimitiveTopology_t::POINTLIST:        topology = GL_POINTS; break;
        case PrimitiveTopology_t::TRIANGLELIST:     topology = GL_TRIANGLES; break;
        case PrimitiveTopology_t::TRIANGLESTRIP:    topology = GL_TRIANGLE_STRIP; break;
    }
    
    return topology;
}

}