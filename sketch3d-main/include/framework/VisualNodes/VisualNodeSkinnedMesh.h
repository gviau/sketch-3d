#ifndef SKETCH_3D_VISUAL_NODE_SKINNED_H
#define SKETCH_3D_VISUAL_NODE_SKINNED_H

#include "framework/VisualNodes/VisualNode.h"

namespace Sketch3D
{
class SkinnedMesh;

class VisualNodeSkinnedMesh : public VisualNode
{
public:
	virtual void					FillRenderingPipelineContext(RenderingPipelineContext& renderingPipelineContext, double deltaTime) override;
	virtual bool					IsNodeValidForRendering() const override { return m_SkinnedMesh.get() != nullptr; }
	virtual void					RenderNode(const shared_ptr<RenderDevice>& renderDevice,
											   const vector<shared_ptr<ConstantBuffer>>& vertexShaderConstantBuffers,
											   const vector<shared_ptr<ConstantBuffer>>& fragmentShaderConstantBuffers,
											   size_t materialConstantsSlot) override;

	void							SetMesh(const shared_ptr<SkinnedMesh>& skinnedMesh) { m_SkinnedMesh = skinnedMesh; }
	const shared_ptr<SkinnedMesh>&	GetMesh() const { return m_SkinnedMesh; }

private:
	shared_ptr<SkinnedMesh>			m_SkinnedMesh;
};
}

#endif