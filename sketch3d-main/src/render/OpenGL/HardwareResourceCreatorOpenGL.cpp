#include "render/OpenGL/HardwareResourceCreatorOpenGL.h"

#include "system/Logger.h"

namespace Sketch3D {
shared_ptr<ConstantBuffer> HardwareResourceCreatorOpenGL::CreateConstantBuffer()  {
    return shared_ptr<ConstantBuffer>();
}

shared_ptr<DepthStencilTarget> HardwareResourceCreatorOpenGL::CreateDepthStencilTarget() {
    return shared_ptr<DepthStencilTarget>();
}

shared_ptr<IndexBuffer> HardwareResourceCreatorOpenGL::CreateIndexBuffer() {
    return shared_ptr<IndexBuffer>();
}

shared_ptr<FragmentShader> HardwareResourceCreatorOpenGL::CreateFragmentShader() {
    return shared_ptr<FragmentShader>();
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
    return shared_ptr<VertexBuffer>();
}

shared_ptr<VertexShader> HardwareResourceCreatorOpenGL::CreateVertexShader() {
    return shared_ptr<VertexShader>();
}
}