#ifndef SKETCH_3D_VISUAL_NODE_MESH_H
#define SKETCH_3D_VISUAL_NODE_MESH_H

#include "framework/VisualNodes/VisualNode.h"

#include <memory>
using namespace std;

namespace Sketch3D
{

class Mesh;

class VisualNodeMesh : public VisualNode
{
public:
	virtual void			FillRenderingPipelineContext(RenderingPipelineContext& renderingPipelineContext, double deltaTime) override;
	virtual bool			IsNodeValidForRendering() const override { return m_Mesh.get() != nullptr; }
	virtual void			RenderNode(const shared_ptr<RenderDevice>& renderDevice,
									   const vector<shared_ptr<ConstantBuffer>>& vertexShaderConstantBuffers,
									   const vector<shared_ptr<ConstantBuffer>>& fragmentShaderConstantBuffers,
									   size_t materialConstantsSlot) override;

	void                    SetMesh(const shared_ptr<Mesh>& mesh) { m_Mesh = mesh; }
	const shared_ptr<Mesh>&	GetMesh() const { return m_Mesh; }

private:
	shared_ptr<Mesh>        m_Mesh;
};
}

#endif