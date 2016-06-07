#ifndef SKETCH_3D_RENDERING_PIPELINE_CONTEXT_H
#define SKETCH_3D_RENDERING_PIPELINE_CONTEXT_H

#include "math/Matrix3x3.h"
#include "math/Matrix4x4.h"

#include "render/ConstantBuffers.h"

#include <vector>
using namespace std;

namespace Sketch3D
{
/**
 * @struct RenderingPipelineContext
 * This struct contains all the information required by the rendering pipeline object to render the scene
 */
struct RenderingPipelineContext
{
	RenderingPipelineContext()
	{
		m_BoneTransformationMatrices.reserve(MAX_BONES);
	}

    Matrix4x4 m_ProjectionMatrix;
    Matrix4x4 m_ViewMatrix;
	Matrix4x4 m_ModelMatrix;

	Matrix4x4 m_ModelViewMatrix;
	Matrix4x4 m_ModelViewProjectionMatrix;

	Matrix4x4 m_TransposedInverseModelViewMatrix;

	vector<Matrix4x4> m_BoneTransformationMatrices;

	// Flags used to determine what to use in the rendering pipeline context object
	union Flags
	{
		struct BitFields
		{
			bool m_IsSkinnedMesh : 1;
			bool m_IsUniformScaling : 1;
		};
	};
	Flags::BitFields m_Flags;
};
}

#endif