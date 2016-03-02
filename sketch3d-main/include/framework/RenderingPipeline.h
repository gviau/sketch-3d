#ifndef SKETCH_3D_RENDERING_PIPELINE_H
#define SKETCH_3D_RENDERING_PIPELINE_H

#include "framework/RenderingPipelineContext.h"

#include <map>
#include <memory>
#include <string>
using namespace std;

namespace Sketch3D
{

class Camera;
class MaterialCodeGenerator;
class RenderContext;
class RenderDevice;
class Scene;

/**
 * @class RenderingPipeline
 * The RenderingPipeline is a base class which can be extend. It defines the different rendering passes to do and in which order.
 */
class RenderingPipeline
{
public:
                                RenderingPipeline(const shared_ptr<RenderContext>& renderContext, const shared_ptr<RenderDevice>& renderDevice);
    virtual                    ~RenderingPipeline();

    virtual bool                Initialize() = 0;

    virtual void                RenderSceneFromCamera(Camera& camera, const Scene& scene) = 0;

    MaterialCodeGenerator*      GetMaterialCodeGenerator() const { return m_MaterialCodeGenerator; }

protected:
    virtual void                ConstructRenderingPipelineContext(Camera& camera, const Scene& scene, RenderingPipelineContext& renderingPipelineContext);

    shared_ptr<RenderContext>   m_RenderContext;
    shared_ptr<RenderDevice>    m_RenderDevice;
    RenderingPipelineContext    m_CurrentRenderingContext;

    MaterialCodeGenerator*      m_MaterialCodeGenerator;
    map<string, size_t>         m_VertexShaderConstantBuffersSlots;
    map<string, size_t>         m_FragmentShaderConstantBuffersSlots;
};
}

#endif