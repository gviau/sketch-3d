#include "framework/RenderingPipeline.h"

#include "framework/Camera.h"
#include "framework/Scene.h"

#include "render/RenderDevice.h"

namespace Sketch3D
{
RenderingPipeline::RenderingPipeline(const shared_ptr<RenderContext>& renderContext, const shared_ptr<RenderDevice>& renderDevice)
    : m_RenderContext(renderContext)
    , m_RenderDevice(renderDevice)
    , m_MaterialCodeGenerator(nullptr)
{
}

RenderingPipeline::~RenderingPipeline()
{
    delete m_MaterialCodeGenerator;
}

void RenderingPipeline::ConstructRenderingPipelineContext(Camera& camera, const Scene& scene, RenderingPipelineContext& renderingPipelineContext)
{
    RenderDevice* renderDevice = m_RenderDevice.get();
    if (renderDevice == nullptr)
    {
        return;
    }

    renderingPipelineContext.m_ProjectionMatrix = renderDevice->GetProjectionMatrix();
    renderingPipelineContext.m_ViewMatrix = camera.GetViewMatrix();
}
}