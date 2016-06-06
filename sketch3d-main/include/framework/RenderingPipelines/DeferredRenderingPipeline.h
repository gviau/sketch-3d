#ifndef SKETCH_3D_DEFERRED_RENDERING_PIPELINE_H
#define SKETCH_3D_DEFERRED_RENDERING_PIPELINE_H

#include "framework/RenderingPipeline.h"

#include <memory>
#include <vector>
using namespace std;

namespace Sketch3D
{

class ConstantBuffer;
class FragmentShader;
class Material;
class RenderTarget;
class SamplerState;
class VertexShader;

class DeferredRenderingPipeline : public RenderingPipeline
{
public:
                                        DeferredRenderingPipeline(const shared_ptr<RenderContext>& renderContext, const shared_ptr<RenderDevice>& renderDevice);
    virtual                            ~DeferredRenderingPipeline();

    virtual bool                        Initialize() override;

    virtual void                        RenderSceneFromCamera(Camera& camera, const Scene& scene, double deltaTime) override;

private:
    shared_ptr<ConstantBuffer>          m_PassConstants;
    shared_ptr<ConstantBuffer>          m_DrawConstants;
	shared_ptr<ConstantBuffer>			m_BoneConstants;
    shared_ptr<ConstantBuffer>          m_LightConstants;

    // GBuffer
    shared_ptr<RenderTarget>            m_PositionsRenderTarget;
    shared_ptr<RenderTarget>            m_NormalsRenderTarget;
    shared_ptr<RenderTarget>            m_AlbedosRenderTarget;
    vector<shared_ptr<RenderTarget>>    m_GBuffer;

    shared_ptr<VertexShader>            m_FullscreenQuadVertexShader;
    shared_ptr<FragmentShader>          m_LightingAccumulationFragmentShader;

    shared_ptr<SamplerState>            m_GBufferSamplerState;

    void                                CreateVertexShader();
    void                                CreateFragmentShader();
};

}

#endif