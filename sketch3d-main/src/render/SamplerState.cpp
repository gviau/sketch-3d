#include "render/SamplerState.h"

namespace Sketch3D {
SamplerState::SamplerState(FilterMode_t filterMode, AddressMode_t addressModeU, AddressMode_t addressModeV, AddressMode_t addressModeW,
                            ComparisonFunction_t comparisonFunction, const Vector4& borderColor) :
        filterMode_(filterMode), addressModeU_(addressModeU), addressModeV_(addressModeV), addressModeW_(addressModeW),
        comparisonFunction_(comparisonFunction), borderColor_(borderColor)
{
}

FilterMode_t SamplerState::GetFilterMode() const {
    return filterMode_;
}

AddressMode_t SamplerState::GetAddressModeU() const {
    return addressModeU_;
}

AddressMode_t SamplerState::GetAddressModeV() const {
    return addressModeV_;
}

AddressMode_t SamplerState::GetAddressModeW() const {
    return addressModeW_;
}

ComparisonFunction_t SamplerState::GetComparisonFunction() const {
    return comparisonFunction_;
}

const Vector4& SamplerState::GetBorderColor() const {
    return borderColor_;
}
}