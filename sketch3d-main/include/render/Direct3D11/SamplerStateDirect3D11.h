#ifndef SKETCH_3D_SAMPLER_STATE_DIRECT3D11_H
#define SKETCH_3D_SAMPLER_STATE_DIRECT3D11_H

#include "render/SamplerState.h"

#pragma warning( disable : 4005 )

#include <d3d11.h>

#pragma warning( default : 4005 )

namespace Sketch3D {
class SKETCH_3D_API SamplerStateDirect3D11 : public SamplerState {
    public:
                            SamplerStateDirect3D11(ID3D11Device* device);
        virtual            ~SamplerStateDirect3D11();

        virtual bool        Initialize(FilterMode_t filterMode, AddressMode_t addressModeU, AddressMode_t addressModeV, AddressMode_t addressModeW,
                                       ComparisonFunction_t comparisonFunction, const Vector4& borderColor=Vector4::ZERO) override;

        ID3D11SamplerState* GetSamplerState() const;

    private:
        ID3D11Device* device_;
        ID3D11SamplerState* samplerState_;
};

D3D11_FILTER GetD3DFilterMode(FilterMode_t filterMode);
D3D11_TEXTURE_ADDRESS_MODE GetD3DAddressMode(AddressMode_t addressMode);
D3D11_COMPARISON_FUNC GetD3DComparisonFunction(ComparisonFunction_t comparisonFunction);
}

#endif