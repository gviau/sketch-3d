#include "render/Direct3D11/RenderDeviceDirect3D11.h"

#include "math/Vector4.h"

#include "render/VertexFormat.h"

#include "render/Direct3D11/BufferDirect3D11.h"
#include "render/Direct3D11/RenderContextDirect3D11.h"
#include "render/Direct3D11/RenderTargetDirect3D11.h"
#include "render/Direct3D11/SamplerStateDirect3D11.h"
#include "render/Direct3D11/ShaderDirect3D11.h"
#include "render/Direct3D11/TextureDirect3D11.h"

#include "system/Logger.h"

#pragma warning( disable : 4005 )

#include <d3d11.h>

#pragma warning( default : 4005 )

namespace Sketch3D {
RenderDeviceDirect3D11::RenderDeviceDirect3D11()
    : device_(nullptr)
    , context_(nullptr)
    , defaultBackbuffer_(nullptr)
    , defaultDepthStencilBuffer_(nullptr)
    , currentRasterizerState_(nullptr)
    , currentDepthStencilState_(nullptr)
    , currentDepthStencilBuffer_(nullptr)
    , hardwareResourceCreator_(nullptr) {
}

RenderDeviceDirect3D11::~RenderDeviceDirect3D11() {
    delete hardwareResourceCreator_;

    if (defaultBackbuffer_ != nullptr) {
        defaultBackbuffer_->Release();
    }

    if (defaultDepthStencilBuffer_ != nullptr) {
        defaultDepthStencilBuffer_->Release();
    }

    if (currentRasterizerState_ != nullptr) {
        currentRasterizerState_->Release();
    }

    if (currentDepthStencilState_ != nullptr) {
        currentDepthStencilState_->Release();
    }
}

bool RenderDeviceDirect3D11::Initialize(const shared_ptr<RenderContext>& renderContext) {
    RenderContextDirect3D11* context = dynamic_cast<RenderContextDirect3D11*>(renderContext.get());
    if (context == nullptr) {
        return false;
    }

    device_ = context->GetDevice();
    context_ = context->GetDeviceContext();
    width_ = context->GetRenderParameters().width;
    height_ = context->GetRenderParameters().height;

    CreateDefaultDepthStencilState(context->GetRenderParameters().depthStencilBits);
    CreateDefaultRasterizerState(renderContext);

    currentRenderTargets_.push_back(defaultBackbuffer_);
    context_->OMSetRenderTargets(1, &defaultBackbuffer_, defaultDepthStencilBuffer_);

    currentDepthStencilBuffer_ = defaultDepthStencilBuffer_;
    context_->OMSetDepthStencilState(currentDepthStencilState_, 0);

    hardwareResourceCreator_ = new HardwareResourceCreatorDirect3D11(device_, context_);

    return true;
}

void RenderDeviceDirect3D11::ClearRenderTargets(const Vector4& color) {
    for (ID3D11RenderTargetView* renderTarget : currentRenderTargets_) {
        context_->ClearRenderTargetView(renderTarget, &color.x);
    }
}

void RenderDeviceDirect3D11::ClearDepthStencil(bool clearDepth, bool clearStencil, float depthValue, unsigned char stencilValue) {
    int clearFlags = ( ( (clearDepth) ? D3D11_CLEAR_DEPTH : 0 ) | ( (clearStencil) ? D3D11_CLEAR_STENCIL : 0 ) );
    context_->ClearDepthStencilView(currentDepthStencilBuffer_, clearFlags, depthValue, stencilValue);
}

void RenderDeviceDirect3D11::SetRenderTargets(const vector<shared_ptr<RenderTarget>>& renderTargets, const shared_ptr<DepthStencilTarget>& depthStencilTarget) {
    currentRenderTargets_.clear();
    for (const shared_ptr<RenderTarget>& renderTarget : renderTargets) {
        RenderTarget* target = renderTarget.get();

        if (target != nullptr) {
            RenderTargetDirect3D11* d3dRenderTarget = static_cast<RenderTargetDirect3D11*>(target);
            currentRenderTargets_.push_back(d3dRenderTarget->GetRenderTargetView());
        }
    }

    DepthStencilTarget* depthStencil = depthStencilTarget.get();
    if (depthStencil != nullptr) {
        DepthStencilTargetDirect3D11* d3dDepthStencilTarget = static_cast<DepthStencilTargetDirect3D11*>(depthStencil);
        currentDepthStencilBuffer_ = d3dDepthStencilTarget->GetDepthStencilView();
    } else {
        currentDepthStencilBuffer_ = defaultDepthStencilBuffer_;
    }

    if (currentRenderTargets_.size() > 0) {
        context_->OMSetRenderTargets(currentRenderTargets_.size(), &currentRenderTargets_[0], currentDepthStencilBuffer_);
    } else {
        currentRenderTargets_.push_back(defaultBackbuffer_);
        context_->OMSetRenderTargets(1, &defaultBackbuffer_, currentDepthStencilBuffer_);
    }
}

void RenderDeviceDirect3D11::SetDepthStencilTarget(const shared_ptr<DepthStencilTarget>& depthStencilTarget) {
    DepthStencilTarget* depthStencil = depthStencilTarget.get();
    if (depthStencil != nullptr) {
        DepthStencilTargetDirect3D11* d3dDepthStencilTarget = static_cast<DepthStencilTargetDirect3D11*>(depthStencil);
        currentDepthStencilBuffer_ = d3dDepthStencilTarget->GetDepthStencilView();
    } else {
        currentDepthStencilBuffer_ = defaultDepthStencilBuffer_;
    }

    context_->OMSetRenderTargets(currentRenderTargets_.size(), &currentRenderTargets_[0], currentDepthStencilBuffer_);
}

void RenderDeviceDirect3D11::SetDefaultRenderTarget() {
    currentRenderTargets_.clear();
    currentRenderTargets_.push_back(defaultBackbuffer_);
    currentDepthStencilBuffer_ = defaultDepthStencilBuffer_;

    context_->OMSetRenderTargets(currentRenderTargets_.size(), &currentRenderTargets_[0], currentDepthStencilBuffer_);
}

void RenderDeviceDirect3D11::SetDepthStencilState(const DepthStencilState_t& depthStencilState, unsigned int referenceMask) {
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = depthStencilState.depthEnable;
    depthStencilDesc.DepthWriteMask = (depthStencilState.depthWriteMask) ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = GetD3DComparisonFunc(depthStencilState.comparisonFunction);
    depthStencilDesc.StencilEnable = depthStencilState.stencilEnable;
    depthStencilDesc.StencilReadMask = depthStencilState.stencilReadMask;
    depthStencilDesc.StencilWriteMask = depthStencilState.stencilWriteMask;

    device_->CreateDepthStencilState(&depthStencilDesc, &currentDepthStencilState_);

    context_->OMSetDepthStencilState(currentDepthStencilState_, referenceMask);
}

void RenderDeviceDirect3D11::SetRasterizerState(const RasterizerState_t& rasterizerState) {
    D3D11_RASTERIZER_DESC rasterizerDesc;
    rasterizerDesc.FillMode = GetD3DFillMode(rasterizerState.fillMode);
    rasterizerDesc.CullMode = GetD3DCullMode(rasterizerState.cullingMethod);
    rasterizerDesc.FrontCounterClockwise = rasterizerState.isFrontFaceCounterClockwise;
    rasterizerDesc.DepthClipEnable = rasterizerState.enableDepthClip;
    rasterizerDesc.ScissorEnable = rasterizerState.enableScissor;
    rasterizerDesc.MultisampleEnable = rasterizerState.enableMultisample;
    rasterizerDesc.AntialiasedLineEnable = rasterizerState.enableAntialiasedLine;

    device_->CreateRasterizerState(&rasterizerDesc, &currentRasterizerState_);

    context_->RSSetState(currentRasterizerState_);
}

void RenderDeviceDirect3D11::SetDefaultDepthStencilState(unsigned int referenceMask) {
    if (currentDepthStencilState_ != nullptr) {
        currentDepthStencilState_->Release();
    }

    SetDepthStencilState(defaultDepthStencilState_, referenceMask);
}

void RenderDeviceDirect3D11::SetDefaultRasterizerState() {
    if (currentRasterizerState_ != nullptr) {
        currentRasterizerState_->Release();
    }

    SetRasterizerState(defaultRasterizerState_);
}

shared_ptr<FragmentShader>& RenderDeviceDirect3D11::GetFragmentShader() {
    return fragmentShader_;
}

void RenderDeviceDirect3D11::SetFragmentShader(shared_ptr<FragmentShader> fragmentShader) {
    if (fragmentShader == nullptr) {
        return;
    }

    fragmentShader_ = fragmentShader;
    FragmentShaderDirect3D11* shader = static_cast<FragmentShaderDirect3D11*>(fragmentShader.get());

    context_->PSSetShader(shader->GetShader(), nullptr, 0);
}

bool RenderDeviceDirect3D11::SetFragmentShaderConstantBuffer(const shared_ptr<ConstantBuffer>& constantBuffer, size_t slot) {
    if (fragmentShader_ == nullptr || constantBuffer == nullptr) {
        return false;
    }

    ConstantBufferDirect3D11* d3dConstantBuffer = static_cast<ConstantBufferDirect3D11*>(constantBuffer.get());

    ID3D11Buffer* const buffer = d3dConstantBuffer->GetBuffer();
    context_->PSSetConstantBuffers(slot, 1, &buffer);

    return true;
}

bool RenderDeviceDirect3D11::SetFragmentShaderSamplerState(const shared_ptr<SamplerState>& state, unsigned int slot) {
    if (fragmentShader_ == nullptr || state == nullptr) {
        return false;
    }

    SamplerStateDirect3D11* d3dSamplerState = static_cast<SamplerStateDirect3D11*>(state.get());

    ID3D11SamplerState* const samplerState = d3dSamplerState->GetSamplerState();
    context_->PSSetSamplers(slot, 1, &samplerState);

    return true;
}

bool RenderDeviceDirect3D11::SetFragmentShaderTexture(const shared_ptr<Texture1D>& texture, unsigned int slot) {
    if (fragmentShader_ == nullptr || texture == nullptr) {
        return false;
    }

    Texture1DDirect3D11* d3dTexture = static_cast<Texture1DDirect3D11*>(texture.get());

    ID3D11ShaderResourceView* const resourceView = d3dTexture->GetShaderResourceView();
    context_->PSSetShaderResources(slot, 1, &resourceView);

    return true;
}

bool RenderDeviceDirect3D11::SetFragmentShaderTexture(const shared_ptr<Texture2D>& texture, unsigned int slot) {
    if (fragmentShader_ == nullptr || texture == nullptr) {
        return false;
    }

    Texture2DDirect3D11* d3dTexture = static_cast<Texture2DDirect3D11*>(texture.get());

    ID3D11ShaderResourceView* const resourceView = d3dTexture->GetShaderResourceView();
    context_->PSSetShaderResources(slot, 1, &resourceView);

    return true;
}

bool RenderDeviceDirect3D11::SetFragmentShaderTexture(const shared_ptr<Texture3D>& texture, unsigned int slot) {
    if (fragmentShader_ == nullptr || texture == nullptr) {
        return false;
    }

    Texture3DDirect3D11* d3dTexture = static_cast<Texture3DDirect3D11*>(texture.get());

    ID3D11ShaderResourceView* const resourceView = d3dTexture->GetShaderResourceView();
    context_->PSSetShaderResources(slot, 1, &resourceView);

    return true;
}

bool RenderDeviceDirect3D11::SetFragmentShaderTexture(const shared_ptr<RenderTarget>& texture, unsigned int slot) {
    if (fragmentShader_ == nullptr || texture == nullptr) {
        return false;
    }

    RenderTargetDirect3D11* renderTarget = static_cast<RenderTargetDirect3D11*>(texture.get());
    
    ID3D11ShaderResourceView* const resourceView = renderTarget->GetResourceView();
    context_->PSSetShaderResources(slot, 1, &resourceView);

    return true;
}

shared_ptr<VertexShader>& RenderDeviceDirect3D11::GetVertexShader() {
    return vertexShader_;
}

void RenderDeviceDirect3D11::SetVertexShader(shared_ptr<VertexShader> vertexShader) {
    if (vertexShader == nullptr) {
        return;
    }

    vertexShader_ = vertexShader;

    VertexShaderDirect3D11* shader = static_cast<VertexShaderDirect3D11*>(vertexShader.get());

    context_->IASetInputLayout(shader->GetInputLayout());
    context_->VSSetShader(shader->GetShader(), nullptr, 0);
}

bool RenderDeviceDirect3D11::SetVertexShaderConstantBuffer(const shared_ptr<ConstantBuffer>& constantBuffer, size_t slot) {
    if (vertexShader_ == nullptr || constantBuffer == nullptr) {
        return false;
    }

    ConstantBufferDirect3D11* d3dConstantBuffer = static_cast<ConstantBufferDirect3D11*>(constantBuffer.get());

    ID3D11Buffer* const buffer = d3dConstantBuffer->GetBuffer();
    context_->VSSetConstantBuffers(slot, 1, &buffer);

    return true;
}

bool RenderDeviceDirect3D11::SetVertexShaderSamplerState(const shared_ptr<SamplerState>& state, unsigned int slot) {
    if (vertexShader_ == nullptr || state == nullptr) {
        return false;
    }

    SamplerStateDirect3D11* d3dSamplerState = static_cast<SamplerStateDirect3D11*>(state.get());

    ID3D11SamplerState* const samplerState = d3dSamplerState->GetSamplerState();
    context_->VSSetSamplers(slot, 1, &samplerState);

    return true;
}

bool RenderDeviceDirect3D11::SetVertexShaderTexture(const shared_ptr<Texture1D>& texture, unsigned int slot) {
    if (vertexShader_ == nullptr || texture == nullptr) {
        return false;
    }

    Texture1DDirect3D11* d3dTexture = static_cast<Texture1DDirect3D11*>(texture.get());

    ID3D11ShaderResourceView* const resourceView = d3dTexture->GetShaderResourceView();
    context_->VSSetShaderResources(slot, 1, &resourceView);

    return true;
}

bool RenderDeviceDirect3D11::SetVertexShaderTexture(const shared_ptr<Texture2D>& texture, unsigned int slot) {
    if (vertexShader_ == nullptr || texture == nullptr) {
        return false;
    }

    Texture2DDirect3D11* d3dTexture = static_cast<Texture2DDirect3D11*>(texture.get());

    ID3D11ShaderResourceView* const resourceView = d3dTexture->GetShaderResourceView();
    context_->VSSetShaderResources(slot, 1, &resourceView);

    return true;
}

bool RenderDeviceDirect3D11::SetVertexShaderTexture(const shared_ptr<Texture3D>& texture, unsigned int slot) {
    if (vertexShader_ == nullptr || texture == nullptr) {
        return false;
    }

    Texture3DDirect3D11* d3dTexture = static_cast<Texture3DDirect3D11*>(texture.get());

    ID3D11ShaderResourceView* const resourceView = d3dTexture->GetShaderResourceView();
    context_->VSSetShaderResources(slot, 1, &resourceView);

    return true;
}

bool RenderDeviceDirect3D11::SetVertexShaderTexture(const shared_ptr<RenderTarget>& texture, unsigned int slot) {
    if (vertexShader_ == nullptr || texture == nullptr) {
        return false;
    }

    RenderTargetDirect3D11* renderTarget = static_cast<RenderTargetDirect3D11*>(texture.get());
    
    ID3D11ShaderResourceView* const resourceView = renderTarget->GetResourceView();
    context_->VSSetShaderResources(slot, 1, &resourceView);

    return true;
}

bool RenderDeviceDirect3D11::Draw(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer, size_t startVertexLocation) {
    D3D11_PRIMITIVE_TOPOLOGY topology = GetD3DPrimitiveTopology(primitiveTopology);
    context_->IASetPrimitiveTopology(topology);

    VertexBufferDirect3D11* d3dVertexBuffer = reinterpret_cast<VertexBufferDirect3D11*>(vertexBuffer.get());
    if (d3dVertexBuffer == nullptr) {
        return false;
    }

    ID3D11Buffer* buffer = d3dVertexBuffer->GetBuffer();
    size_t stride = vertexBuffer->GetVertexFormat()->GetSize();
    size_t offset = 0;
    context_->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
    context_->Draw(vertexBuffer->GetNumVertices(), startVertexLocation);

    return true;
}

bool RenderDeviceDirect3D11::DrawIndexed(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer,
                                         const shared_ptr<IndexBuffer>& indexBuffer, size_t startIndexLocation, int baseVertexLocation) {
    D3D11_PRIMITIVE_TOPOLOGY topology = GetD3DPrimitiveTopology(primitiveTopology);
    context_->IASetPrimitiveTopology(topology);

    VertexBufferDirect3D11* d3dVertexBuffer = reinterpret_cast<VertexBufferDirect3D11*>(vertexBuffer.get());
    if (d3dVertexBuffer == nullptr) {
        return false;
    }

    IndexBufferDirect3D11* d3dIndexBuffer = reinterpret_cast<IndexBufferDirect3D11*>(indexBuffer.get());
    if (d3dIndexBuffer == nullptr) {
        return false;
    }

    ID3D11Buffer* vBuffer = d3dVertexBuffer->GetBuffer();
    ID3D11Buffer* iBuffer = d3dIndexBuffer->GetBuffer();

    size_t stride = vertexBuffer->GetVertexFormat()->GetSize();
    size_t offset = 0;
    context_->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);

    DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;
    if (indexBuffer->GetIndexFormat() == IndexFormat_t::INT_2_BYTES) {
        format = DXGI_FORMAT_R16_UINT;
    }
    context_->IASetIndexBuffer(iBuffer, format, 0);

    context_->DrawIndexed(indexBuffer->GetNumIndices(), startIndexLocation, baseVertexLocation);

    return true;
}

bool RenderDeviceDirect3D11::DrawInstanced(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer, const vector<Matrix4x4>& modelMatrices) {
    D3D11_PRIMITIVE_TOPOLOGY topology = GetD3DPrimitiveTopology(primitiveTopology);
    context_->IASetPrimitiveTopology(topology);

    return true;
}

bool RenderDeviceDirect3D11::DrawIndexedInstanced(PrimitiveTopology_t primitiveTopology, const shared_ptr<VertexBuffer>& vertexBuffer,
                                                  const shared_ptr<IndexBuffer>& indexBuffer, const vector<Matrix4x4>& modelMatrices) {
    D3D11_PRIMITIVE_TOPOLOGY topology = GetD3DPrimitiveTopology(primitiveTopology);
    context_->IASetPrimitiveTopology(topology);

    return true;
}

void RenderDeviceDirect3D11::GenerateMips(Texture* texture) {
}

void RenderDeviceDirect3D11::CopyResource(const shared_ptr<HardwareResource>& source, const shared_ptr<HardwareResource>& destination) {
}

HardwareResourceCreator* RenderDeviceDirect3D11::GetHardwareResourceCreator() const {
    return hardwareResourceCreator_;
}

bool RenderDeviceDirect3D11::CreateDefaultDepthStencilState(DepthStencilBits_t depthStencilBits) {
    // Create the default depth stencil buffer
    DXGI_FORMAT depthStencilFormat;
    switch (depthStencilBits) {
        case DepthStencilBits_t::D16:   depthStencilFormat = DXGI_FORMAT_D16_UNORM; break;
        case DepthStencilBits_t::D24S8: depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; break;
        case DepthStencilBits_t::D32:   depthStencilFormat = DXGI_FORMAT_D32_FLOAT; break;
    }

    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width = width_;
    depthStencilDesc.Height = height_;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = depthStencilFormat;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    ID3D11Texture2D* depthStencilTexture;
    HRESULT result = device_->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilTexture);
    if (FAILED(result)) {
        Logger::GetInstance()->Error("Couldn't create texture for depth stencil buffer");
        return false;
    }

    result = device_->CreateDepthStencilView(depthStencilTexture, nullptr, &defaultDepthStencilBuffer_);
    depthStencilTexture->Release();

    if (FAILED(result)) {
        Logger::GetInstance()->Error("Couldn't create depth stencil buffer");
        return false;
    }

    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
    depthStencilStateDesc.DepthEnable = true;
    depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilStateDesc.StencilEnable = false;
    depthStencilStateDesc.StencilReadMask = 0;
    depthStencilStateDesc.StencilWriteMask = 0;

    result = device_->CreateDepthStencilState(&depthStencilStateDesc, &currentDepthStencilState_);
    if (FAILED(result)) {
        Logger::GetInstance()->Error("Couldn't create the depth stencil state");
        return false;
    }

    return true;
}

bool RenderDeviceDirect3D11::CreateDefaultRasterizerState(const shared_ptr<RenderContext>& renderContext) {
    RenderContextDirect3D11* context = static_cast<RenderContextDirect3D11*>(renderContext.get());
    // Create the default back buffer
    ID3D11Texture2D* backbuffer;
    HRESULT result = context->swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);
    if (FAILED(result)) {
        Logger::GetInstance()->Error("Couldn't retrieve the backbuffer from the swap chain");
        return false;
    }

    result = device_->CreateRenderTargetView(backbuffer, nullptr, &defaultBackbuffer_);
    if (FAILED(result)) {
        Logger::GetInstance()->Error("Couldn't create the default render target view");
        return false;
    }
    backbuffer->Release();

    // Set the default rasterizer state
    D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

    result = device_->CreateRasterizerState(&rasterizerDesc, &currentRasterizerState_);
    if (FAILED(result)) {
        Logger::GetInstance()->Error("Couldn't create the default rasterizer state");
        return false;
    }

    context_->RSSetState(currentRasterizerState_);
    
    return true;
}

D3D11_COMPARISON_FUNC GetD3DComparisonFunc(ComparisonFunction_t func) {
    D3D11_COMPARISON_FUNC comp;
    switch (func) {
        case ComparisonFunction_t::ALWAYS:          comp = D3D11_COMPARISON_ALWAYS; break;
        case ComparisonFunction_t::EQUAL:           comp = D3D11_COMPARISON_EQUAL; break;
        case ComparisonFunction_t::GREATER:         comp = D3D11_COMPARISON_GREATER; break;
        case ComparisonFunction_t::GREATER_EQUAL:   comp = D3D11_COMPARISON_GREATER_EQUAL; break;
        case ComparisonFunction_t::LESS:            comp = D3D11_COMPARISON_LESS; break;
        case ComparisonFunction_t::LESS_EQUAL:      comp = D3D11_COMPARISON_LESS_EQUAL; break;
        case ComparisonFunction_t::NEVER:           comp = D3D11_COMPARISON_NEVER; break;
        case ComparisonFunction_t::NOT_EQUAL:       comp = D3D11_COMPARISON_NOT_EQUAL; break;
    }

    return comp;
}

D3D11_FILL_MODE GetD3DFillMode(FillMode_t mode) {
    D3D11_FILL_MODE fillMode;
    switch (mode) {
        case FillMode_t::FILL:      fillMode = D3D11_FILL_SOLID; break;
        case FillMode_t::WIREFRAME: fillMode = D3D11_FILL_WIREFRAME; break;
    }

    return fillMode;
}

D3D11_CULL_MODE GetD3DCullMode(CullingMethod_t method) {
    D3D11_CULL_MODE cullMode;
    switch (method) {
        case CullingMethod_t::BACK_FACE:    cullMode = D3D11_CULL_BACK; break;
        case CullingMethod_t::FRONT_FACE:   cullMode = D3D11_CULL_FRONT; break;
    }

    return cullMode;
}

D3D11_PRIMITIVE_TOPOLOGY GetD3DPrimitiveTopology(PrimitiveTopology_t primitiveTopology) {
    D3D11_PRIMITIVE_TOPOLOGY d3dPrimitiveTopology;
    switch (primitiveTopology) {
        case PrimitiveTopology_t::LINELIST:         d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST; break;
        case PrimitiveTopology_t::LINESTRIP:        d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
        case PrimitiveTopology_t::POINTLIST:        d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST; break;
        case PrimitiveTopology_t::TRIANGLELIST:     d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
        case PrimitiveTopology_t::TRIANGLESTRIP:    d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
    }
    
    return d3dPrimitiveTopology;
}

}