#include "framework/VisualNodes/VisualNodeMesh.h"

#include "framework/Mesh.h"
#include "framework/RenderingPipelineContext.h"

namespace Sketch3D
{
void VisualNodeMesh::FillRenderingPipelineContext(RenderingPipelineContext& renderingPipelineContext, double deltaTime)
{
	FillRenderingPipelineContextCommon(renderingPipelineContext);

	renderingPipelineContext.m_Flags.m_IsSkinnedMesh = false;
}

void VisualNodeMesh::RenderNode(const shared_ptr<RenderDevice>& renderDevice,
								const vector<shared_ptr<ConstantBuffer>>& vertexShaderConstantBuffers,
								const vector<shared_ptr<ConstantBuffer>>& fragmentShaderConstantBuffers,
								size_t materialConstantsSlot)
{
	Mesh* meshToDraw = m_Mesh.get();

	meshToDraw->SetConstantBuffersForAllMaterials(vertexShaderConstantBuffers, fragmentShaderConstantBuffers, materialConstantsSlot);

	meshToDraw->SetAmbientColorForAllSubMeshes(GetAmbientColor());
	meshToDraw->SetDiffuseColorForAllSubMeshes(GetDiffuseColor());
	meshToDraw->SetSpecularColorForAllSubMeshes(GetSpecularColor());
	meshToDraw->SetSpecularPowerForAllSubMeshes(GetSpecularPower());

	// Draw the mesh
	meshToDraw->Draw(renderDevice);
}
}