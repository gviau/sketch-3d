#ifndef SKETCH_3D_SAMPLER_STATE_H
#define SKETCH_3D_SAMPLER_STATE_H

#include "system/Platform.h"

#include "math/Vector4.h"

#include "render/RenderDevice_Common.h"

namespace Sketch3D {

/**
 * @enum FilterMode_t
 * The different types of filtering mode for a sampler state
 */
enum FilterMode_t {
    NEAREST,
    LINEAR
};

/**
 * @enum AddressMode_t
 * The different of address modes for the sampler state, that is, how they will access the pixels depending on
 * the texture coordinates
 */
enum AddressMode_t {
    WRAP,
    MIRROR,
    CLAMP,
    BORDER
};

/**
 * @class SamplerState
 * Base class of the sampler state, which is used in conjunction with a texture to describe how
 * to sample from it
 */
class SKETCH_3D_API SamplerState {
    public:
                                SamplerState(FilterMode_t filterMode, AddressMode_t addressModeU, AddressMode_t addressModeV, AddressMode_t addressModeW,
                                             ComparisonFunction_t comparisonFunction, const Vector4& borderColor=Vector4::ZERO);
        virtual                ~SamplerState() {}

        FilterMode_t            GetFilterMode() const;
        AddressMode_t           GetAddressModeU() const;
        AddressMode_t           GetAddressModeV() const;
        AddressMode_t           GetAddressModeW() const;
        ComparisonFunction_t    GetComparisonFunction() const;
        const Vector4&          GetBorderColor() const;

    protected:
        FilterMode_t            filterMode_;
        AddressMode_t           addressModeU_;
        AddressMode_t           addressModeV_;
        AddressMode_t           addressModeW_;
        ComparisonFunction_t    comparisonFunction_;
        Vector4                 borderColor_;
};
}

#endif