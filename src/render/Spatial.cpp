#include "Spatial.h"

#include "RenderSystem.h"

namespace Sketch3D
{
Spatial::Spatial(const string& name) : Node(name),
                                       transformMatrix_(Matrix4x4::IDENTITY)
{
}

void Spatial::Draw(RenderSystem* renderSystem) const
{
    renderSystem->SetTransformMatrix(transformMatrix_);
}

void Spatial::SetTransformation(const Matrix4x4& transform)
{
    transformMatrix_ = transform;
}

Matrix4x4 Spatial::GetTransformation() const
{
    return transformMatrix_;
}

Spatial::nodeType_t Spatial::GetType() const
{
    return SPATIAL;
}
}
