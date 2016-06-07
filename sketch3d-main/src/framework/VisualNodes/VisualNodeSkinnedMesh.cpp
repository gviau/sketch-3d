#include "framework/VisualNodes/VisualNodeSkinnedMesh.h"

#include "framework/RenderingPipelineContext.h"
#include "framework/SkinnedMesh.h"

namespace Sketch3D
{
void VisualNodeSkinnedMesh::FillRenderingPipelineContext(RenderingPipelineContext& renderingPipelineContext, double deltaTime)
{
	FillRenderingPipelineContextCommon(renderingPipelineContext);

	renderingPipelineContext.m_Flags.m_IsSkinnedMesh = true;

	m_SkinnedMesh->Animate(deltaTime);
	renderingPipelineContext.m_BoneTransformationMatrices = m_SkinnedMesh->GetTransformationMatrices();
}

void VisualNodeSkinnedMesh::RenderNode(const shared_ptr<RenderDevice>& renderDevice,
									   const vector<shared_ptr<ConstantBuffer>>& vertexShaderConstantBuffers,
									   const vector<shared_ptr<ConstantBuffer>>& fragmentShaderConstantBuffers,
									   size_t materialConstantsSlot)
{
	Mesh* meshToDraw = m_SkinnedMesh.get();

	meshToDraw->SetConstantBuffersForAllMaterials(vertexShaderConstantBuffers, fragmentShaderConstantBuffers, materialConstantsSlot);

	meshToDraw->SetAmbientColorForAllSubMeshes(GetAmbientColor());
	meshToDraw->SetDiffuseColorForAllSubMeshes(GetDiffuseColor());
	meshToDraw->SetSpecularColorForAllSubMeshes(GetSpecularColor());
	meshToDraw->SetSpecularPowerForAllSubMeshes(GetSpecularPower());

	// Draw the mesh
	meshToDraw->Draw(renderDevice);
}
}