#ifndef SKECTH_3D_RENDER_STATE_CACHE_H
#define SKETCH_3D_RENDER_STATE_CACHE_H

#include "render/RenderState.h"

#include "system/Platform.h"

namespace Sketch3D {

/**
 * @class RenderStateCache
 * This class is used to state the render state. It doesn't immediately apply the states but instead
 * store the new value and apply it through the device when the actual rendering occurs.
 */
class SKETCH_3D_API RenderStateCache {
    public:
        /**
         * Constructor. Sets all the states to their default values
         */
                            RenderStateCache();
        virtual            ~RenderStateCache();

        void                ApplyRenderStateChanges();
        void                ApplyClearStateChanges();

        void                EnableDepthTest(bool val);
        void                EnableDepthWrite(bool val);
        void                EnableColorWrite(bool val);
        void                EnableBlending(bool val);
        void                SetDepthComparisonFunc(DepthFunc_t comparison);
        void                SetCullingMethod(CullingMethod_t cullingMethod);
        void                SetBlendingEquation(BlendingEquation_t equation);
        void                SetBlendingFactor(BlendingFactor_t srcFactor, BlendingFactor_t dstFactor);
        void                SetRenderFillMode(FillMode_t mode);

    protected:
        // Current render state values
        bool                isDepthTestEnabled_;
        bool                isDepthWriteEnabled_;
        bool                isColorWriteEnabled_;
        bool                isBlendingEnabled_;
        DepthFunc_t         depthComparisonFunction_;
        CullingMethod_t     cullingMethod_;
        BlendingEquation_t  blendingEquation_;
        BlendingFactor_t    sourceBlendingFactor_;
        BlendingFactor_t    destinationBlendingFactor_;
        FillMode_t          renderMode_;

        // Old render state values
        bool                oldIsDepthTestEnabled_;
        bool                oldIsDepthWriteEnabled_;
        bool                oldIsColorWriteEnabled_;
        bool                oldIsBlendingEnabled_;
        DepthFunc_t         oldDepthComparisonFunction_;
        CullingMethod_t     oldCullingMethod_;
        BlendingEquation_t  oldBlendingEquation_;
        BlendingFactor_t    oldSourceBlendingFactor_;
        BlendingFactor_t    oldDestinationBlendingFactor_;
        FillMode_t          oldRenderMode_;

        virtual void        EnableDepthTestImpl() = 0;
        virtual void        EnableDepthWriteImpl() = 0;
        virtual void        EnableColorWriteImpl() = 0;
        virtual void        EnableBlendingImpl() = 0;
        virtual void        SetDepthComparisonFuncImpl() = 0;
        virtual void        SetCullingMethodImpl() = 0;
        virtual void        SetBlendingEquationImpl() = 0;
        virtual void        SetBlendingFactorImpl() = 0;
        virtual void        SetRenderFillModeImpl() = 0;
};

}

#endif