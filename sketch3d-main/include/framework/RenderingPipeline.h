#ifndef SKETCH_3D_RENDERING_PIPELINE_H
#define SKETCH_3D_RENDERING_PIPELINE_H

#include "framework/RenderingPipelineContext.h"

namespace Sketch3D
{

class Camera;
class Scene;

/**
 * @class RenderingPipeline
 * The RenderingPipeline is a base class which can be extend. It defines the different rendering passes to do and in which order.
 */
class RenderingPipeline
{
public:
    virtual void                RenderSceneFromCamera(const Camera& camera, const Scene& scene) = 0;

protected:
    virtual void                ConstructRenderingPipelineContext(const Camera& camera, const Scene& scene, RenderingPipelineContext& renderingPipelineContext);

    RenderingPipelineContext    m_CurrentRenderingContext;
};
}

#endif