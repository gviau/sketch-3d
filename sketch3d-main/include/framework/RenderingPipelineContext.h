#ifndef SKETCH_3D_RENDERING_PIPELINE_CONTEXT_H
#define SKETCH_3D_RENDERING_PIPELINE_CONTEXT_H

#include "math/Matrix3x3.h"
#include "math/Matrix4x4.h"

namespace Sketch3D
{
/**
 * @struct RenderingPipelineContext
 * This struct contains all the information required by the rendering pipeline object to render the scene
 */
struct RenderingPipelineContext
{
    Matrix4x4 m_ProjectionMatrix;
    Matrix4x4 m_ViewMatrix;
};
}

#endif