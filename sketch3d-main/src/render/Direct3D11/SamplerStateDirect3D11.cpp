#include "render/Direct3D11/SamplerStateDirect3D11.h"

#include "system/Logger.h"

namespace Sketch3D {
SamplerStateDirect3D11::SamplerStateDirect3D11(ID3D11Device* device, FilterMode_t filterMode, AddressMode_t addressModeU, AddressMode_t addressModeV,
                                                AddressMode_t addressModeW, ComparisonFunction_t comparisonFunction, const Vector4& borderColor) :
        SamplerState(filterMode, addressModeU, addressModeV, addressModeW, comparisonFunction, borderColor), samplerState_(nullptr)
{
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = GetD3DFilterMode(filterMode);
    samplerDesc.AddressU = GetD3DAddressMode(addressModeU_);
    samplerDesc.AddressV = GetD3DAddressMode(addressModeV_);
    samplerDesc.AddressW = GetD3DAddressMode(addressModeW_);
    samplerDesc.BorderColor[0] = borderColor_.x;
    samplerDesc.BorderColor[1] = borderColor_.y;
    samplerDesc.BorderColor[2] = borderColor_.z;
    samplerDesc.BorderColor[3] = borderColor_.w;
    samplerDesc.ComparisonFunc = GetD3DComparisonFunction(comparisonFunction_);
    samplerDesc.MaxAnisotropy = 1.0f;
    samplerDesc.MaxLOD = FLT_MAX;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MipLODBias = 0.0f;

    HRESULT hr = device->CreateSamplerState(&samplerDesc, &samplerState_);
    if (FAILED(hr)) {
        Logger::GetInstance()->Error("Couldn't create sampler state");
    }
}

SamplerStateDirect3D11::~SamplerStateDirect3D11() {
    if (samplerState_ != nullptr) {
        samplerState_->Release();
    }
}

ID3D11SamplerState* SamplerStateDirect3D11::GetSamplerState() const {
    return samplerState_;
}

D3D11_FILTER GetD3DFilterMode(FilterMode_t filterMode) {
    D3D11_FILTER filter;
    switch (filterMode) {
        case FilterMode_t::NEAREST: filter = D3D11_FILTER_MIN_MAG_MIP_POINT; break;
        case FilterMode_t::LINEAR:  filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
    }

    return filter;
}

D3D11_TEXTURE_ADDRESS_MODE GetD3DAddressMode(AddressMode_t addressMode) {
    D3D11_TEXTURE_ADDRESS_MODE address;
    switch (addressMode) {
        case AddressMode_t::BORDER: address = D3D11_TEXTURE_ADDRESS_BORDER; break;
        case AddressMode_t::CLAMP:  address = D3D11_TEXTURE_ADDRESS_CLAMP; break;
        case AddressMode_t::MIRROR: address = D3D11_TEXTURE_ADDRESS_MIRROR; break;
        case AddressMode_t::WRAP:   address = D3D11_TEXTURE_ADDRESS_WRAP; break;
    }

    return address;
}

D3D11_COMPARISON_FUNC GetD3DComparisonFunction(ComparisonFunction_t comparisonFunction) {
    D3D11_COMPARISON_FUNC comparison;
    switch (comparisonFunction) {
        case ComparisonFunction_t::ALWAYS:          comparison = D3D11_COMPARISON_ALWAYS; break;
        case ComparisonFunction_t::EQUAL:           comparison = D3D11_COMPARISON_EQUAL; break;
        case ComparisonFunction_t::GREATER:         comparison = D3D11_COMPARISON_GREATER; break;
        case ComparisonFunction_t::GREATER_EQUAL:   comparison = D3D11_COMPARISON_GREATER_EQUAL; break;
        case ComparisonFunction_t::LESS:            comparison = D3D11_COMPARISON_LESS; break;
        case ComparisonFunction_t::LESS_EQUAL:      comparison = D3D11_COMPARISON_LESS_EQUAL; break;
        case ComparisonFunction_t::NEVER:           comparison = D3D11_COMPARISON_NEVER; break;
        case ComparisonFunction_t::NOT_EQUAL:       comparison = D3D11_COMPARISON_NOT_EQUAL; break;
    }

    return comparison;
}

}