#ifndef SKETCH_3D_RENDER_STATE_CACHE_DIRECT3D9_H
#define SKETCH_3D_RENDER_STATE_CACHE_DIRECT3D9_H

#include "render/RenderStateCache.h"

struct IDirect3DDevice9;

namespace Sketch3D {
/**
 * @class RenderStateCacheDirect3D9
 * Direct3D9 implementation of the RenderStateCache
 */
class RenderStateCacheDirect3D9 : public RenderStateCache {
    public:
                            RenderStateCacheDirect3D9(IDirect3DDevice9* device);
        virtual            ~RenderStateCacheDirect3D9();

    protected:
        virtual void        EnableDepthTestImpl();
        virtual void        EnableDepthWriteImpl();
        virtual void        EnableColorWriteImpl();
        virtual void        EnableBlendingImpl();
        virtual void        SetDepthComparisonFuncImpl();
        virtual void        SetCullingMethodImpl();
        virtual void        SetBlendingEquationImpl();
        virtual void        SetBlendingFactorImpl();
        virtual void        SetRenderFillModeImpl();

    private:
        IDirect3DDevice9*   device_;
        unsigned int        GetBlendingFactor(BlendingFactor_t factor) const;
};

}

#endif