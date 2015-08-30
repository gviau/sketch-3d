#include "render/OpenGL/RenderDeviceOpenGL.h"

#include "math/Vector4.h"

#include "render/OpenGL/BufferOpenGL.h"
#include "render/OpenGL/ErrorCheckingOpenGL.h"
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
    GL_CALL( glDeleteProgramPipelines(1, &pipeline_) );
    delete hardwareResourceCreator_;
}

bool RenderDeviceOpenGL::Initialize(const shared_ptr<RenderContext>& renderContext)
{
    hardwareResourceCreator_ = new HardwareResourceCreatorOpenGL;

    width_ = renderContext->GetRenderParameters().width;
    height_ = renderContext->GetRenderParameters().height;

    GL_CALL( glEnable(GL_CULL_FACE) );

    CreateDefaultDepthStencilState(renderContext->GetRenderParameters().depthStencilBits);
    CreateDefaultRasterizerState(renderContext);

    GL_CALL( glGenProgramPipelines(1, &pipeline_) );
    GL_CALL( glUseProgram(0) );

    return true;
}

void RenderDeviceOpenGL::ClearRenderTargets(const Vector4& color)
{
    GL_CALL( glClearColor(color.x, color.y, color.z, color.w) );
    GL_CALL( glClear(GL_COLOR_BUFFER_BIT) );
}

void RenderDeviceOpenGL::ClearDepthStencil(bool clearDepth, bool clearStencil, float depthValue, unsigned char stencilValue)
{
    int clearFlags = ( ( (clearDepth) ? GL_DEPTH_BUFFER_BIT : 0 ) | ( (clearStencil) ? GL_STENCIL_BUFFER_BIT : 0 ) );

    if (clearDepth)
    {
        GL_CALL( glClearDepth(depthValue) );
    }

    if (clearStencil)
    {
        GL_CALL( glClearStencil(stencilValue) );
    }

    GL_CALL( glClear(clearFlags) );
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
    if (depthStencilState.depthEnable)
    {
        GL_CALL( glEnable(GL_DEPTH_TEST) );
    }
    else
    {
        GL_CALL( glDisable(GL_DEPTH_TEST) );
    }

    GL_CALL( glDepthMask( (depthStencilState.depthWriteMask) ? GL_TRUE : GL_FALSE ) );
    GL_CALL( glDepthFunc( GetOGLComparisonFunc(depthStencilState.comparisonFunction) ) );

    if (depthStencilState.stencilEnable)
    {
        GL_CALL( glEnable(GL_STENCIL_TEST) );
    }
    else
    {
        GL_CALL( glDisable(GL_STENCIL_TEST) );
    }
}

void RenderDeviceOpenGL::SetRasterizerState(const RasterizerState_t& rasterizerState)
{
    GL_CALL( glPolygonMode(GL_FRONT_AND_BACK, GetOGLFillMode(rasterizerState.fillMode)) );
    GL_CALL( glCullFace(GetOGLCullMode(rasterizerState.cullingMethod)) );
    
    if (rasterizerState.isFrontFaceCounterClockwise)
    {
        GL_CALL( glFrontFace(GL_CCW) );
    }
    else
    {
        GL_CALL( glFrontFace(GL_CW) );
    }
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
    
    GL_CALL( glBindProgramPipeline(pipeline_) );
    GL_CALL( glUseProgramStages(pipeline_, GL_FRAGMENT_SHADER_BIT, fragmentShaderOGL->GetShader()) );
    GL_CALL( glActiveShaderProgram(pipeline_, fragmentShaderOGL->GetShader()) );
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
    
    ConstantBufferOpenGL* constantBufferOGL = dynamic_cast<ConstantBufferOpenGL*>(constantBufferPtr);

    GL_CALL( glBindBufferRange(GL_UNIFORM_BUFFER, slot, constantBufferOGL->GetBuffer(), 0, constantBufferOGL->GetBufferSizeInBytes()) );

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
    
    GL_CALL( glBindProgramPipeline(pipeline_) );
    GL_CALL( glUseProgramStages(pipeline_, GL_VERTEX_SHADER_BIT, vertexShaderOGL->GetShader()) );
    GL_CALL( glActiveShaderProgram(pipeline_, vertexShaderOGL->GetShader()) );
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
    
    ConstantBufferOpenGL* constantBufferOGL = dynamic_cast<ConstantBufferOpenGL*>(constantBufferPtr);

    GL_CALL( glBindBufferRange(GL_UNIFORM_BUFFER, slot, constantBufferOGL->GetBuffer(), 0, constantBufferOGL->GetBufferSizeInBytes()) );

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
        GL_CALL( glGenVertexArrays(1, &vertexArrayObject_) );
    }

    VertexShaderOpenGL* vertexShader = dynamic_cast<VertexShaderOpenGL*>(vertexShader_.get());
    VertexFormat* vertexFormat = vertexShader->GetVertexFormat();
    const vector<InputLayout_t>& inputLayouts = vertexFormat->GetInputLayouts();

    GL_CALL( glBindVertexArray(vertexArrayObject_) );

    VertexBufferOpenGL* vertexBufferOGL = dynamic_cast<VertexBufferOpenGL*>(vertexBuffer.get());
    if (vertexBufferOGL == nullptr)
    {
        return false;
    }

    GLuint buffer = vertexBufferOGL->GetBuffer();
    GL_CALL( glBindBuffer(GL_ARRAY_BUFFER, buffer) );
    
    // This is not very nice :/ have to find a better way
    size_t idx = 0;
    for (InputLayout_t inputLayout : inputLayouts) {
        GL_CALL( glEnableVertexAttribArray(idx) );
        GL_CALL( glVertexAttribPointer(idx, GetFormatSize(inputLayout.format), GetOGLFormat(inputLayout.format), GL_FALSE, vertexFormat->GetSize(), (void*)inputLayout.byteOffset) );

        idx += 1;
    }

    GLenum topology = GetOGLTopology(primitiveTopology);

    GL_CALL( glDrawArrays(topology, startVertexLocation, vertexBuffer->GetNumVertices()) );

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
        GL_CALL( glGenVertexArrays(1, &vertexArrayObject_) );
    }

    VertexShaderOpenGL* vertexShader = dynamic_cast<VertexShaderOpenGL*>(vertexShader_.get());
    VertexFormat* vertexFormat = vertexShader->GetVertexFormat();
    const vector<InputLayout_t>& inputLayouts = vertexFormat->GetInputLayouts();

    GL_CALL( glBindVertexArray(vertexArrayObject_) );

    VertexBufferOpenGL* vertexBufferOGL = dynamic_cast<VertexBufferOpenGL*>(vertexBuffer.get());
    if (vertexBufferOGL == nullptr)
    {
        return false;
    }

    GLuint buffer = vertexBufferOGL->GetBuffer();
    GL_CALL( glBindBuffer(GL_ARRAY_BUFFER, buffer) );
    
    // This is not very nice :/ have to find a better way
    size_t idx = 0;
    for (InputLayout_t inputLayout : inputLayouts) {
        GL_CALL( glEnableVertexAttribArray(idx) );
        GL_CALL( glVertexAttribPointer(idx, GetFormatSize(inputLayout.format), GetOGLFormat(inputLayout.format), GL_FALSE, vertexFormat->GetSize(), (void*)inputLayout.byteOffset) );

        idx += 1;
    }

    IndexBufferOpenGL* indexBufferOGL = dynamic_cast<IndexBufferOpenGL*>(indexBuffer.get());
    if (indexBufferOGL == nullptr)
    {
        return false;
    }

    buffer = indexBufferOGL->GetBuffer();
    GL_CALL( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer) );

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

    GL_CALL( glDrawElements(topology, indexBuffer->GetNumIndices(), type, nullptr) );

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

Matrix4x4 RenderDeviceOpenGL::CalculatePerspectiveProjection(float width, float height, float nearPlane, float farPlane)
{
    projection_[0][0] = 2.0f * nearPlane / width;
    projection_[1][1] = 2.0f * nearPlane / height;
    projection_[2][2] = - (farPlane + nearPlane) / (farPlane - nearPlane);
    projection_[2][3] = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);
    projection_[3][2] = -1.0f;
    projection_[3][3] = 0.0f;

    return projection_;
}

Matrix4x4 RenderDeviceOpenGL::CalculatePerspectiveProjectionFOV(float fov, float aspectRatio, float nearPlane, float farPlane)
{
    float yScale = 1.0f / tanf(fov / 2.0f);
    float xScale = yScale / aspectRatio;

    projection_[0][0] = xScale;
    projection_[1][1] = yScale;
    projection_[2][2] = - (farPlane + nearPlane) / (farPlane - nearPlane);
    projection_[2][3] = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);
    projection_[3][2] = -1.0f;
    projection_[3][3] = 0.0f;

    return projection_;
}

HardwareResourceCreator* RenderDeviceOpenGL::GetHardwareResourceCreator() const
{
    return hardwareResourceCreator_;
}

bool RenderDeviceOpenGL::CreateDefaultDepthStencilState(DepthStencilBits_t depthStencilBits)
{
    // Set the default depth stencil state
    defaultDepthStencilState_.depthEnable = true;
    defaultDepthStencilState_.depthWriteMask = true;
    defaultDepthStencilState_.comparisonFunction = ComparisonFunction_t::LESS;
    defaultDepthStencilState_.stencilEnable = false;
    defaultDepthStencilState_.stencilReadMask = 0;
    defaultDepthStencilState_.stencilWriteMask = 0;

    SetDepthStencilState(defaultDepthStencilState_, 0);

    return true;
}

bool RenderDeviceOpenGL::CreateDefaultRasterizerState(const shared_ptr<RenderContext>& renderContext)
{
    // Set the default rasterizer state
    defaultRasterizerState_.enableAntialiasedLine = false;
    defaultRasterizerState_.cullingMethod = CullingMethod_t::BACK_FACE;
    defaultRasterizerState_.fillMode = FillMode_t::FILL;
    defaultRasterizerState_.isFrontFaceCounterClockwise = true;
    defaultRasterizerState_.enableScissor = false;
    defaultRasterizerState_.enableMultisample = false;
    defaultRasterizerState_.enableDepthClip = true;

    SetRasterizerState(defaultRasterizerState_);

    return true;
}

GLenum GetOGLComparisonFunc(ComparisonFunction_t func)
{
    GLenum comp;
    switch (func) {
        case ComparisonFunction_t::ALWAYS:          comp = GL_ALWAYS; break;
        case ComparisonFunction_t::EQUAL:           comp = GL_EQUAL; break;
        case ComparisonFunction_t::GREATER:         comp = GL_GREATER; break;
        case ComparisonFunction_t::GREATER_EQUAL:   comp = GL_GEQUAL; break;
        case ComparisonFunction_t::LESS:            comp = GL_LESS; break;
        case ComparisonFunction_t::LESS_EQUAL:      comp = GL_LEQUAL; break;
        case ComparisonFunction_t::NEVER:           comp = GL_NEVER; break;
        case ComparisonFunction_t::NOT_EQUAL:       comp = GL_NOTEQUAL; break;
    }

    return comp;
}

GLenum GetOGLFillMode(FillMode_t mode)
{
    GLenum fillMode;
    switch (mode) {
        case FillMode_t::FILL:      fillMode = GL_FILL; break;
        case FillMode_t::WIREFRAME: fillMode = GL_LINE; break;
    }

    return fillMode;
}

GLenum GetOGLCullMode(CullingMethod_t method)
{
    GLenum cullMode;
    switch (method) {
        case CullingMethod_t::BACK_FACE:    cullMode = GL_BACK; break;
        case CullingMethod_t::FRONT_FACE:   cullMode = GL_FRONT; break;
    }

    return cullMode;
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