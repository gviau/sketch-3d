#ifndef SKETCH_3D_FORWARD_RENDERING_PIPELINE_H
#define SKETCH_3D_FORWARD_RENDERING_PIPELINE_H

#include "framework/RenderingPipeline.h"

#include <memory>
using namespace std;

namespace Sketch3D
{

class ConstantBuffer;

class ForwardRenderingPipeline : public RenderingPipeline
{
public:
                    ForwardRenderingPipeline(const shared_ptr<RenderContext>& renderContext, const shared_ptr<RenderDevice>& renderDevice);
    virtual        ~ForwardRenderingPipeline();

    virtual bool    Initialize() override;

    virtual void    RenderSceneFromCamera(Camera& camera, const Scene& scene, double deltaTime) override;

private:
    shared_ptr<ConstantBuffer>  m_PassConstants;
    shared_ptr<ConstantBuffer>  m_DrawConstants;
	shared_ptr<ConstantBuffer>	m_BoneConstants;
    shared_ptr<ConstantBuffer>  m_LightConstants;
};
}

#endif