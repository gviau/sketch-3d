#include "framework/Scene.h"

namespace Sketch3D
{
// Dummy visual node class to be able to instantiate the root node
class VisualNodeRoot : public VisualNode
{
public:
	virtual void	FillRenderingPipelineContext(RenderingPipelineContext& renderingPipelineContext, double deltaTime) override {}
	virtual void	RenderNode(const shared_ptr<RenderDevice>& renderDevice,
							   const vector<shared_ptr<ConstantBuffer>>& vertexShaderConstantBuffers,
							   const vector<shared_ptr<ConstantBuffer>>& fragmentShaderConstantBuffers,
							   size_t materialConstantsSlot) override {}
};

Scene::Scene()
{
	m_RootNode = new VisualNodeRoot;
}

Scene::~Scene()
{
	delete m_RootNode;
}
}