#include "render/OpenGL/HardwareResourceCreatorOpenGL.h"

#include "render/OpenGL/BufferOpenGL.h"
#include "render/OpenGL/ShaderOpenGL.h"

#include "system/Logger.h"

namespace Sketch3D {
shared_ptr<ConstantBuffer> HardwareResourceCreatorOpenGL::CreateConstantBuffer()  {
    ConstantBuffer* constantBuffer = new ConstantBufferOpenGL;
    return shared_ptr<ConstantBuffer>(constantBuffer);
}

shared_ptr<DepthStencilTarget> HardwareResourceCreatorOpenGL::CreateDepthStencilTarget() {
    return shared_ptr<DepthStencilTarget>();
}

shared_ptr<IndexBuffer> HardwareResourceCreatorOpenGL::CreateIndexBuffer() {
    IndexBuffer* indexBuffer = new IndexBufferOpenGL;
    return shared_ptr<IndexBuffer>(indexBuffer);
}

shared_ptr<FragmentShader> HardwareResourceCreatorOpenGL::CreateFragmentShader() {
    FragmentShader* fragmentShader = new FragmentShaderOpenGL;
    return shared_ptr<FragmentShader>(fragmentShader);
}

shared_ptr<RenderTarget> HardwareResourceCreatorOpenGL::CreateRenderTarget() {
    return shared_ptr<RenderTarget>();
}

shared_ptr<SamplerState> HardwareResourceCreatorOpenGL::CreateSamplerState() {
    return shared_ptr<SamplerState>();
}

shared_ptr<Texture2D> HardwareResourceCreatorOpenGL::CreateTexture2D() {
    return shared_ptr<Texture2D>();
}

shared_ptr<VertexBuffer> HardwareResourceCreatorOpenGL::CreateVertexBuffer() {
    VertexBuffer* vertexBuffer = new VertexBufferOpenGL;
    return shared_ptr<VertexBuffer>(vertexBuffer);
}

shared_ptr<VertexShader> HardwareResourceCreatorOpenGL::CreateVertexShader() {
    VertexShader* vertexShader = new VertexShaderOpenGL;
    return shared_ptr<VertexShader>(vertexShader);
}
}