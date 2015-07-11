#include "render/RenderStateCache.h"

namespace Sketch3D {
RenderStateCache::RenderStateCache() {
    oldCullingMethod_ = cullingMethod_ = CullingMethod_t::BACK_FACE;
    oldIsDepthTestEnabled_ = isDepthTestEnabled_ = true;
    oldDepthComparisonFunction_ = depthComparisonFunction_ = DEPTH_FUNC_LESS;
    oldRenderMode_ = renderMode_ = FillMode_t::FILL;

    oldIsDepthWriteEnabled_ = isDepthWriteEnabled_ = true;
    oldIsColorWriteEnabled_ = isColorWriteEnabled_ = true;
    oldIsBlendingEnabled_ = isBlendingEnabled_ = false;

    oldBlendingEquation_ = blendingEquation_ = BLENDING_EQUATION_ADD;
    oldSourceBlendingFactor_ = sourceBlendingFactor_ = BLENDING_FACTOR_ONE;
    oldDestinationBlendingFactor_ = destinationBlendingFactor_ = BLENDING_FACTOR_ZERO;
}

RenderStateCache::~RenderStateCache() {

}

void RenderStateCache::ApplyRenderStateChanges() {
    ApplyClearStateChanges();

    if (oldIsDepthTestEnabled_ != isDepthTestEnabled_) {
        oldIsDepthTestEnabled_ = isDepthTestEnabled_;
        EnableDepthTestImpl();
    }

    if (oldIsBlendingEnabled_ != isBlendingEnabled_) {
        oldIsBlendingEnabled_ = isBlendingEnabled_;
        EnableBlendingImpl();
    }

    if (oldDepthComparisonFunction_ != depthComparisonFunction_) {
        oldDepthComparisonFunction_ = depthComparisonFunction_;
        SetDepthComparisonFuncImpl();
    }

    if (oldCullingMethod_ != cullingMethod_) {
        oldCullingMethod_ = cullingMethod_;
        SetCullingMethodImpl();
    }

    if (oldBlendingEquation_ != blendingEquation_) {
        oldBlendingEquation_ = blendingEquation_;
        SetBlendingEquationImpl();
    }

    if (oldSourceBlendingFactor_ != sourceBlendingFactor_ || oldDestinationBlendingFactor_ != destinationBlendingFactor_) {
        oldSourceBlendingFactor_ = sourceBlendingFactor_;
        oldDestinationBlendingFactor_ = destinationBlendingFactor_;
        SetBlendingFactorImpl();
    }

    if (oldRenderMode_ != renderMode_) {
        oldRenderMode_ = renderMode_;
        SetRenderFillModeImpl();
    }
}

void RenderStateCache::ApplyClearStateChanges() {
    if (oldIsDepthWriteEnabled_ != isDepthWriteEnabled_) {
        oldIsDepthWriteEnabled_ = isDepthWriteEnabled_;
        EnableDepthWriteImpl();
    }

    if (oldIsColorWriteEnabled_ != isColorWriteEnabled_) {
        oldIsColorWriteEnabled_ = isColorWriteEnabled_;
        EnableColorWriteImpl();
    }
}

void RenderStateCache::EnableDepthTest(bool val) {
    isDepthTestEnabled_ = val;
}

void RenderStateCache::EnableDepthWrite(bool val) {
    isDepthWriteEnabled_ = val;
}

void RenderStateCache::EnableColorWrite(bool val) {
    isColorWriteEnabled_ = val;
}

void RenderStateCache::EnableBlending(bool val) {
    isBlendingEnabled_ = val;
}

void RenderStateCache::SetDepthComparisonFunc(DepthFunc_t comparison) {
    depthComparisonFunction_ = comparison;
}

void RenderStateCache::SetCullingMethod(CullingMethod_t cullingMethod) {
    cullingMethod_ = cullingMethod;
}

void RenderStateCache::SetBlendingEquation(BlendingEquation_t equation) {
    blendingEquation_ = equation;
}

void RenderStateCache::SetBlendingFactor(BlendingFactor_t srcFactor, BlendingFactor_t dstFactor) {
    sourceBlendingFactor_ = srcFactor;
    destinationBlendingFactor_ = dstFactor;
}

void RenderStateCache::SetRenderFillMode(FillMode_t mode) {
    renderMode_ = mode;
}

}