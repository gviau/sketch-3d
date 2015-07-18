#include "render/SamplerState.h"

namespace Sketch3D {
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