#include "render/Direct3D9/RenderSystemDirect3D9.h"

#include "render/Direct3D9/BufferObjectManagerDirect3D9.h"
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
    delete renderContext_;
}

bool RenderSystemDirect3D9::Initialize() {
    Logger::GetInstance()->Info("Initializing Direct3D9");

    renderContext_ = new RenderContextDirect3D9;
    if (!renderContext_->Initialize(window_)) {
        Logger::GetInstance()->Error("Couldn't create Direct3D9 context");
        return false;
    }

    device_ = renderContext_->GetDevice();

    QueryDeviceCapabilities();

    bufferObjectManager_ = new BufferObjectManagerDirect3D9(device_);

    // Retrieve the current depth buffer and render target
    device_->GetDepthStencilSurface(&depthBuffer_);
    device_->GetRenderTarget(0, &renderTarget_);

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

void RenderSystemDirect3D9::Render() {

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

void RenderSystemDirect3D9::SetRenderFillMode(RenderMode_t mode) const {
    DWORD renderMode;
    switch (mode) {
        case RENDER_MODE_FILL:
            renderMode = D3DFILL_SOLID;
            break;

        case RENDER_MODE_POINT:
            renderMode = D3DFILL_POINT;
            break;

        case RENDER_MODE_WIREFRAME:
            renderMode = D3DFILL_WIREFRAME;
            break;
    }

    device_->SetRenderState(D3DRS_FILLMODE, renderMode);
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

void RenderSystemDirect3D9::EnableDepthTest(bool val) const {
    device_->SetRenderState(D3DRS_ZENABLE, (val) ? D3DZB_TRUE : D3DZB_FALSE);
}

void RenderSystemDirect3D9::EnableDepthWrite(bool val) const {
    device_->SetRenderState(D3DRS_ZWRITEENABLE, (val) ? TRUE : FALSE);
}

void RenderSystemDirect3D9::EnableColorWrite(bool val) const {
    device_->SetRenderState(D3DRS_COLORWRITEENABLE, (val) ? TRUE : FALSE);
}

void RenderSystemDirect3D9::SetDepthComparisonFunc(DepthFunc_t comparison) const {
    DWORD func;
    switch (comparison) {
        case DEPTH_FUNC_NEVER:
            func = D3DCMP_NEVER;
            break;

        case DEPTH_FUNC_ALWAYS:
            func = D3DCMP_ALWAYS;
            break;

        case DEPTH_FUNC_LESS:
            func = D3DCMP_LESS;
            break;

        case DEPTH_FUNC_LEQUAL:
            func = D3DCMP_LESSEQUAL;
            break;

        case DEPTH_FUNC_EQUAL:
            func = D3DCMP_EQUAL;
            break;

        case DEPTH_FUNC_GEQUAL:
            func = D3DCMP_GREATEREQUAL;
            break;

        case DEPTH_FUNC_GREATER:
            func = D3DCMP_GREATER;
            break;

        case DEPTH_FUNC_NOTEQUAL:
            func = D3DCMP_NOTEQUAL;
            break;
    }

    device_->SetRenderState(D3DRS_ZFUNC, func);
}

void RenderSystemDirect3D9::SetCullingMethod(CullingMethod_t cullingMethod) const {
    DWORD cullMode;
    if (cullingMethod == CULLING_METHOD_BACK_FACE) {
        cullMode = D3DCULL_CW;
    } else if (cullingMethod == CULLING_METHOD_FRONT_FACE) {
        cullMode = D3DCULL_CCW;
    }

    device_->SetRenderState(D3DRS_CULLMODE, cullMode);
}

Shader* RenderSystemDirect3D9::CreateShader(const string& vertexFilename,  const string& fragmentFilename) {
    shaders_.push_back(new ShaderDirect3D9(vertexFilename + ".hlsl", fragmentFilename + ".hlsl", device_));
    return shaders_[shaders_.size() - 1];
}

Texture2D* RenderSystemDirect3D9::CreateTexture2D() const {
    return new Texture2DDirect3D9(device_);
}

Texture3D* RenderSystemDirect3D9::CreateTexture3D() const {
    return nullptr;
}

RenderTexture* RenderSystemDirect3D9::CreateRenderTexture(unsigned int width, unsigned int height, TextureFormat_t format) const {
    return new RenderTextureDirect3D9(device_, width, height, format);
}

void RenderSystemDirect3D9::BindScreenBuffer() const {
    // Set the depth buffer and render target back to what they were, invalidating the other render targets that may have been set earlier
    device_->SetDepthStencilSurface(depthBuffer_);
    device_->SetRenderTarget(0, renderTarget_);

    for (int i = 1; i < deviceCapabilities_.maxNumberRenderTargets_; i++) {
        device_->SetRenderTarget(i, nullptr);
    }
}

void RenderSystemDirect3D9::EnableBlending(bool val) const {
    device_->SetRenderState(D3DRS_ALPHABLENDENABLE, (val) ? TRUE : FALSE);
}

void RenderSystemDirect3D9::SetBlendingEquation(BlendingEquation_t equation) const {
    DWORD blendingEquation;
    switch (equation) {
        case BLENDING_EQUATION_ADD:
            blendingEquation = D3DBLENDOP_ADD;
            break;

        case BLENDING_EQUATION_SUBTRACT:
            blendingEquation = D3DBLENDOP_SUBTRACT;
            break;

        case BLENDING_EQUATION_REVERSE_SUBTRACT:
            blendingEquation = D3DBLENDOP_REVSUBTRACT;
            break;

        case BLENDING_EQUATION_MIN:
            blendingEquation = D3DBLENDOP_MIN;
            break;

        case BLENDING_EQUATION_MAX:
            blendingEquation = D3DBLENDOP_MAX;
            break;
    }

    device_->SetRenderState(D3DRS_BLENDFACTOR, blendingEquation);
}

void RenderSystemDirect3D9::SetBlendingFactor(BlendingFactor_t srcFactor, BlendingFactor_t destFactor) const {
    DWORD srcBlendFactor = GetBlendingFactor(srcFactor);
    DWORD dstBlendFactor = GetBlendingFactor(destFactor);

    device_->SetRenderState(D3DRS_SRCBLEND, srcBlendFactor);
    device_->SetRenderState(D3DRS_DESTBLEND, dstBlendFactor);
}

void RenderSystemDirect3D9::BindTexture(const Texture* texture, size_t unit) const {
}

void RenderSystemDirect3D9::BindShader(const Shader* shader) {
    if (shader == nullptr) {
        device_->SetVertexShader(nullptr);
        device_->SetPixelShader(nullptr);
    } else {
        const ShaderDirect3D9* shaderDirect3D9 = static_cast<const ShaderDirect3D9*>(shader);
        device_->SetVertexShader(shaderDirect3D9->vertexShader_);
        device_->SetPixelShader(shaderDirect3D9->fragmentShader_);
    }
}

unsigned int RenderSystemDirect3D9::GetBlendingFactor(BlendingFactor_t factor) const {
    unsigned int blendingFactor;
    switch (factor) {
        case BLENDING_FACTOR_ZERO:
            blendingFactor = D3DBLEND_ZERO;
            break;

        case BLENDING_FACTOR_ONE:
            blendingFactor = D3DBLEND_ONE;
            break;

        case BLENDING_FACTOR_SRC_COLOR:
            blendingFactor = D3DBLEND_SRCCOLOR;
            break;

        case BLENDING_FACTOR_ONE_MINUS_SRC_COLOR:
            blendingFactor = D3DBLEND_INVSRCCOLOR;
            break;

        case BLENDING_FACTOR_DST_COLOR:
            blendingFactor = D3DBLEND_DESTCOLOR;
            break;

        case BLENDING_FACTOR_ONE_MINUS_DST_COLOR:
            blendingFactor = D3DBLEND_INVDESTCOLOR;
            break;

        case BLENDING_FACTOR_SRC_ALPHA:
            blendingFactor = D3DBLEND_SRCALPHA;
            break;

        case BLENDING_FACTOR_ONE_MINUS_SRC_ALPHA:
            blendingFactor = D3DBLEND_INVSRCALPHA;
            break;

        case BLENDING_FACTOR_DST_ALPHA:
            blendingFactor = D3DBLEND_DESTALPHA;
            break;

        case BLENDING_FACTOR_ONE_MINUS_DST_ALPHA:
            blendingFactor = D3DBLEND_INVDESTALPHA;
            break;

        case BLENDING_FACTOR_SRC_ALPHA_SATURATE:
            blendingFactor = D3DBLEND_SRCALPHASAT;
            break;
    }

    return blendingFactor;
}

void RenderSystemDirect3D9::QueryDeviceCapabilities() {
    D3DCAPS9 caps;
    device_->GetDeviceCaps(&caps);
    deviceCapabilities_.maxActiveTextures_ = caps.MaxTextureBlendStages;
    deviceCapabilities_.maxNumberRenderTargets_ = caps.NumSimultaneousRTs;
}

}