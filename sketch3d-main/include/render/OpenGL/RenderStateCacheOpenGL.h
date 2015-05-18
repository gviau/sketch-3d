#ifndef SKETCH_3D_RENDER_STATE_CACHE_OPENGL_H
#define SKETCH_3D_RENDER_STATE_CACHE_OPENGL_H

#include "render/RenderStateCache.h"

namespace Sketch3D {

/**
 * @class RenderStateCacheOpenGL
 * OpenGL implementation of the RenderStateCache
 */
class RenderStateCacheOpenGL : public RenderStateCache {
    public:
                        RenderStateCacheOpenGL();
        virtual        ~RenderStateCacheOpenGL();

    protected:
        virtual void    EnableDepthTestImpl();
        virtual void    EnableDepthWriteImpl();
        virtual void    EnableColorWriteImpl();
        virtual void    EnableBlendingImpl();
        virtual void    SetDepthComparisonFuncImpl();
        virtual void    SetCullingMethodImpl();
        virtual void    SetBlendingEquationImpl();
        virtual void    SetBlendingFactorImpl();
        virtual void    SetRenderFillModeImpl();

    private:
        unsigned int    GetBlendingFactor(BlendingFactor_t factor) const;
};
}

#endif