#include "render/Direct3D9/RenderSystemDirect3D9.h"

#include "render/Direct3D9/BufferObjectManagerDirect3D9.h"
#include "render/Direct3D9/RenderStateCacheDirect3D9.h"
#include "render/Direct3D9/RenderTextureDirect3D9.h"
#include "render/Direct3D9/ShaderDirect3D9.h"
#include "render/Direct3D9/Texture2DDirect3D9.h"

#include "system/Logger.h"

#include <d3dx9.h>

namespace Sketch3D {

RenderSystemDirect3D9::RenderSystemDirect3D9(Window& window) : RenderSystem(window), renderContext_(nullptr), device_(nullptr) {
    Logger::GetInstance()->Info("Current rendering API: Direct3D9");

    clearColor_.r = clearColor_.g = clearColor_.b = 0.0f;
    clearColor_.a = 1.0f;
}

RenderSystemDirect3D9::~RenderSystemDirect3D9() {
    Logger::GetInstance()->Info("Shutdown Direct3D9");
    device_->SetVertexShader(nullptr);
    device_->SetPixelShader(nullptr);
    FreeRenderSystem();
    delete renderContext_;
}

bool RenderSystemDirect3D9::Initialize(const RenderParameters_t& renderParameters) {
    Logger::GetInstance()->Info("Initializing Direct3D9");

    renderContext_ = new RenderContextDirect3D9;
    if (!renderContext_->Initialize(window_, renderParameters)) {
        Logger::GetInstance()->Error("Couldn't create Direct3D9 context");
        return false;
    }

    device_ = renderContext_->GetDevice();

    QueryDeviceCapabilities();

    bufferObjectManager_ = new BufferObjectManagerDirect3D9(device_);
    renderStateCache_ = new RenderStateCacheDirect3D9(device_);

    // Retrieve the current depth buffer and render target
    device_->GetDepthStencilSurface(&depthBuffer_);
    device_->GetRenderTarget(0, &renderTarget_);

    CreateTextShader();

    return true;
}

void RenderSystemDirect3D9::SetClearColor(float red, float green, float blue, float alpha) {
    clearColor_.r = red;
    clearColor_.g = green;
    clearColor_.b = blue;
    clearColor_.a = alpha;
}

void RenderSystemDirect3D9::Clear(int buffer) const {
    DWORD flags = 0;
    if ((buffer & CLEAR_BUFFER_COLOR) > 0) {
        flags |= D3DCLEAR_TARGET;
    }

    if ((buffer & CLEAR_BUFFER_DEPTH) > 0) {
        flags |= D3DCLEAR_ZBUFFER;
    }
    
    device_->Clear(0, nullptr, flags, D3DCOLOR_COLORVALUE(clearColor_.r, clearColor_.g, clearColor_.b, clearColor_.a), 1.0f, 0);
}

void RenderSystemDirect3D9::StartRender() {
    device_->BeginScene();
}

void RenderSystemDirect3D9::EndRender() {
    device_->EndScene();
}

void RenderSystemDirect3D9::PresentFrame() {
    renderContext_->SwapBuffers();
}

Matrix4x4 RenderSystemDirect3D9::OrthoProjection(float left, float right, float bottom, float top,
                                                 float nearPlane, float farPlane) const
{
    float w = right - left;
    float h = top - bottom;
    float dz = nearPlane - farPlane;

    Matrix4x4 projection;
    projection[0][0] = 2.0f / w;
    projection[1][1] = 2.0f / h;
    projection[2][2] = 1.0f / dz;
    projection[2][3] = nearPlane / dz;
    return projection;
}

Matrix4x4 RenderSystemDirect3D9::PerspectiveProjection(float left, float right, float bottom, float top,
                                                       float nearPlane, float farPlane) const
{
    float w = right - left;
    float h = top - bottom;
    float dz = nearPlane - farPlane;
    float z2 = 2.0f * nearPlane;

    Matrix4x4 projection;
    projection[0][0] = z2 / w;
    projection[1][1] = z2 / h;
    projection[2][2] = farPlane / dz;
    projection[3][2] = -1.0f;
    projection[2][3] = nearPlane * farPlane / dz;
    projection[3][3] = 0.0f;

    return projection;
}

void RenderSystemDirect3D9::SetViewport(size_t x, size_t y, size_t width, size_t height) {
    D3DVIEWPORT9 viewport;
    viewport.X = x;
    viewport.Y = y;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;
    device_->SetViewport(&viewport);
}

Shader* RenderSystemDirect3D9::CreateShader() {
    shaders_.push_back(new ShaderDirect3D9(device_));
    return shaders_.back();
}

Texture2D* RenderSystemDirect3D9::CreateTexture2D() const {
    return new Texture2DDirect3D9(device_);
}

Texture3D* RenderSystemDirect3D9::CreateTexture3D() const {
    return nullptr;
}

RenderTexture* RenderSystemDirect3D9::CreateRenderTexture(unsigned int width, unsigned int height, TextureFormat_t format) {
    renderTextures_.push_back(new RenderTextureDirect3D9(device_, width, height, format));
    return renderTextures_.back();
}

void RenderSystemDirect3D9::BindScreenBuffer() const {
    // Set the depth buffer and render target back to what they were, invalidating the other render targets that may have been set earlier
    device_->SetDepthStencilSurface(depthBuffer_);
    device_->SetRenderTarget(0, renderTarget_);

    for (int i = 1; i < deviceCapabilities_.maxNumberRenderTargets_; i++) {
        device_->SetRenderTarget(i, nullptr);
    }
}

void RenderSystemDirect3D9::BindShader(const Shader* shader) {
    if (shader != boundShader_) {
        if (shader == nullptr) {
            device_->SetVertexShader(nullptr);
            device_->SetPixelShader(nullptr);
        } else {
            const ShaderDirect3D9* shaderDirect3D9 = static_cast<const ShaderDirect3D9*>(shader);
            device_->SetVertexShader(shaderDirect3D9->vertexShader_);
            device_->SetPixelShader(shaderDirect3D9->fragmentShader_);
        }

        boundShader_ = shader;
    }
}

FrustumPlanes_t RenderSystemDirect3D9::ExtractViewFrustumPlanes(const Matrix4x4& viewProjection) const {
    FrustumPlanes_t frustumPlanes;

    Vector3 nearPlaneNormal(viewProjection[2][0], viewProjection[2][1], viewProjection[2][2]);
    float nearPlaneLength = nearPlaneNormal.Length();
    frustumPlanes.nearPlane.SetNormalizedNormal(nearPlaneNormal / nearPlaneLength);
    frustumPlanes.nearPlane.SetDistance( (viewProjection[3][3] + viewProjection[2][3]) / nearPlaneLength );

    Vector3 farPlaneNormal(viewProjection[3][0] - viewProjection[2][0], viewProjection[3][1] - viewProjection[2][1], viewProjection[3][2] - viewProjection[2][2]);
    float farPlaneLength = farPlaneNormal.Length();
    frustumPlanes.farPlane.SetNormalizedNormal(farPlaneNormal / farPlaneLength);
    frustumPlanes.farPlane.SetDistance( (viewProjection[3][3] - viewProjection[2][3]) / farPlaneLength );

    Vector3 leftPlaneNormal(viewProjection[3][0] + viewProjection[0][0], viewProjection[3][1] + viewProjection[0][1], viewProjection[3][2] + viewProjection[0][2]);
    float leftPlaneLength = leftPlaneNormal.Length();
    frustumPlanes.leftPlane.SetNormalizedNormal(leftPlaneNormal / leftPlaneLength);
    frustumPlanes.leftPlane.SetDistance( (viewProjection[3][3] + viewProjection[0][3]) / leftPlaneLength );

    Vector3 rightPlaneNormal(viewProjection[3][0] - viewProjection[0][0], viewProjection[3][1] - viewProjection[0][1], viewProjection[3][2] - viewProjection[0][2]);
    float rightPlaneLength = rightPlaneNormal.Length();
    frustumPlanes.rightPlane.SetNormalizedNormal(rightPlaneNormal / rightPlaneLength);
    frustumPlanes.rightPlane.SetDistance( (viewProjection[3][3] - viewProjection[0][3]) / rightPlaneLength );

    Vector3 bottomPlaneNormal(viewProjection[3][0] + viewProjection[1][0], viewProjection[3][1] + viewProjection[1][1], viewProjection[3][2] + viewProjection[1][2]);
    float bottomPlaneLength = bottomPlaneNormal.Length();
    frustumPlanes.bottomPlane.SetNormalizedNormal(bottomPlaneNormal / bottomPlaneLength);
    frustumPlanes.bottomPlane.SetDistance( (viewProjection[3][3] + viewProjection[1][3]) / bottomPlaneLength );

    Vector3 topPlaneNormal(viewProjection[3][0] - viewProjection[1][0], viewProjection[3][1] - viewProjection[1][1], viewProjection[3][2] - viewProjection[1][2]);
    float topPlaneLength = topPlaneNormal.Length();
    frustumPlanes.topPlane.SetNormalizedNormal(topPlaneNormal / topPlaneLength);
    frustumPlanes.topPlane.SetDistance( (viewProjection[3][3] - viewProjection[1][3]) / topPlaneLength );

    return frustumPlanes;
}

void RenderSystemDirect3D9::QueryDeviceCapabilities() {
    D3DCAPS9 caps;
    device_->GetDeviceCaps(&caps);
    deviceCapabilities_.maxActiveTextures_ = caps.MaxTextureBlendStages;
    deviceCapabilities_.maxNumberRenderTargets_ = caps.NumSimultaneousRTs;
}

void RenderSystemDirect3D9::CreateTextShader() {
    const char* textVertexShaderSource = \
        "struct VS_INPUT {\n"
            "float3 in_vertex : POSITION;\n"
            "float2 in_uv : TEXCOORD;\n"
        "};\n"

        "struct VS_OUTPUT {\n"
            "float4 position : POSITION;\n"
            "float2 uv : TEXCOORD;\n"
        "};\n"

        "VS_OUTPUT main(VS_INPUT input) {\n"
            "VS_OUTPUT output;\n"
            "output.uv = input.in_uv - float2(-0.0001, 0.0001);\n"  // Offsetting the uv in the texture seems to solve a little artifact
            "output.position = float4(input.in_vertex.xy, 0.0, 1.0);\n"
            "return output;\n"
        "}"
    ;

    const char* textFragmentShaderSource = \
        "sampler2D fontAtlas;\n"
        "float3 textColor;\n"

        "struct PS_INPUT {\n"
            "float4 position : POSITION;\n"
            "float2 uv : TEXCOORD;\n"
        "};\n"

        "float4 main(PS_INPUT input) : COLOR {\n"
            "float val = tex2D(fontAtlas, input.uv).a;\n"
            "if (val < 0.5) {\n"
                "discard;\n"
            "}\n"

            "return float4(textColor, 1.0);\n"
        "}"
    ;

    textShader_ = Renderer::GetInstance()->CreateShader();
    if (!textShader_->SetSource(textVertexShaderSource, textFragmentShaderSource)) {
        Logger::GetInstance()->Error("Couldn't create text shader");
    }
}

}