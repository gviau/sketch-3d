#include "render/Direct3D11/RenderDeviceDirect3D11.h"

#include "math/Vector4.h"

#include "render/VertexFormat.h"

#include "render/Direct3D11/BufferDirect3D11.h"
#include "render/Direct3D11/RenderContextDirect3D11.h"
#include "render/Direct3D11/ShaderDirect3D11.h"

#pragma warning( disable : 4005 )

#include <d3d11.h>

#pragma warning( default : 4005 )

namespace Sketch3D {
RenderDeviceDirect3D11::RenderDeviceDirect3D11() : device_(nullptr), context_(nullptr), currentBackbuffer_(nullptr), currentDepthStencilBuffer_(nullptr), hardwareResourceCreator_(nullptr) {
}

RenderDeviceDirect3D11::~RenderDeviceDirect3D11() {
    delete hardwareResourceCreator_;
}

bool RenderDeviceDirect3D11::Initialize(const shared_ptr<RenderContext>& renderContext) {
    RenderContextDirect3D11* context = dynamic_cast<RenderContextDirect3D11*>(renderContext.get());
    if (context == nullptr) {
        return false;
    }

    device_ = context->GetDevice();
    context_ = context->GetDeviceContext();
    currentBackbuffer_ = context->GetBackBuffer();
    currentDepthStencilBuffer_ = context->GetDepthStencilBuffer();

    hardwareResourceCreator_ = new HardwareResourceCreatorDirect3D11(device_, context_);

    renderTargets_.push_back(currentBackbuffer_);

    context_->OMSetRenderTargets(1, &currentBackbuffer_, currentDepthStencilBuffer_);

    return true;
}

void RenderDeviceDirect3D11::ClearRenderTargets(const Vector4& color) {
    for (ID3D11RenderTargetView* renderTarget : renderTargets_) {
        context_->ClearRenderTargetView(renderTarget, &color.x);
    }
}

void RenderDeviceDirect3D11::ClearDepthStencil(bool clearDepth, bool clearStencil, float depthValue, unsigned char stencilValue) {
    int clearFlags = ( ( (clearDepth) ? D3D11_CLEAR_DEPTH : 0 ) | ( (clearStencil) ? D3D11_CLEAR_STENCIL : 0 ) );
    context_->ClearDepthStencilView(currentDepthStencilBuffer_, clearFlags, depthValue, stencilValue);
}

void RenderDeviceDirect3D11::SetRenderTargets(const vector<RenderTarget*>& renderTargets, DepthStencilTarget* depthStencilTarget) {
}

void RenderDeviceDirect3D11::SetDepthStencilState(const DepthStencilState_t& depthStencilState, unsigned int referenceMask) {
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = depthStencilState.depthEnable;
    depthStencilDesc.DepthWriteMask = (depthStencilState.depthWriteMask) ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = GetD3DComparisonFunc(depthStencilState.comparisonFunction);
    depthStencilDesc.StencilEnable = depthStencilState.stencilEnable;
    depthStencilDesc.StencilReadMask = depthStencilState.stencilReadMask;
    depthStencilDesc.StencilWriteMask = depthStencilState.stencilWriteMask;

    ID3D11DepthStencilState* d3dDepthStencilState;
    device_->CreateDepthStencilState(&depthStencilDesc, &d3dDepthStencilState);

    context_->OMSetDepthStencilState(d3dDepthStencilState, referenceMask);
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

    ID3D11RasterizerState* d3dRestarizerState;
    device_->CreateRasterizerState(&rasterizerDesc, &d3dRestarizerState);

    context_->RSSetState(d3dRestarizerState);
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

void RenderDeviceDirect3D11::SetFragmentShaderConstantBuffer(const shared_ptr<ConstantBuffer>& constantBuffer) {
    if (fragmentShader_ == nullptr || constantBuffer == nullptr) {
        return;
    }

    ConstantBufferDirect3D11* d3dConstantBuffer = static_cast<ConstantBufferDirect3D11*>(constantBuffer.get());

    ID3D11Buffer* const buffer = d3dConstantBuffer->GetBuffer();
    context_->PSSetConstantBuffers(0, 1, &buffer);
}

void RenderDeviceDirect3D11::SetFragmentShaderSamplerState(SamplerState* state, unsigned int slot) {
}

void RenderDeviceDirect3D11::SetFragmentShaderTexture(Texture* texture, unsigned int slot) {
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

void RenderDeviceDirect3D11::SetVertexShaderConstantBuffer(const shared_ptr<ConstantBuffer>& constantBuffer) {
    if (vertexShader_ == nullptr || constantBuffer == nullptr) {
        return;
    }

    ConstantBufferDirect3D11* d3dConstantBuffer = static_cast<ConstantBufferDirect3D11*>(constantBuffer.get());

    ID3D11Buffer* const buffer = d3dConstantBuffer->GetBuffer();
    context_->VSSetConstantBuffers(0, 1, &buffer);
}

void RenderDeviceDirect3D11::SetVertexShaderSamplerState(SamplerState* state, unsigned int slot) {
}

void RenderDeviceDirect3D11::SetVertexShaderTexture(Texture* texture, unsigned int slot) {
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