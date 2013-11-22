#include "Geometry.h"

#include "Renderer.h"
#include "Spatial.h"
#include "Vector3.h"

namespace Sketch3D
{
Geometry::Geometry(const string& name) : Node(name)
{
}

void Geometry::Draw(RenderSystem* renderSystem) const
{
}

void Geometry::Translate(float x, float y, float z)
{
    Spatial* parent = GetSpatialParent();

    Matrix4x4 newTransform = parent->GetTransformation();
    newTransform(0, 3) += x;
    newTransform(1, 3) += y;
    newTransform(2, 3) += z;

    parent->SetTransformation(newTransform);
}

void Geometry::Translate(const Vector3& translation)
{
    Translate(translation.x, translation.y, translation.z);
}

void Geometry::RotateX(float angle)
{
    Spatial* parent = GetSpatialParent();

    Matrix4x4 newTransform = parent->GetTransformation();
    Matrix4x4 rotation;
    rotation.RotationAroundX(angle);
    newTransform *= rotation;

    parent->SetTransformation(newTransform);
}

void Geometry::RotateY(float angle)
{
    Spatial* parent = GetSpatialParent();

    Matrix4x4 newTransform = parent->GetTransformation();
    Matrix4x4 rotation;
    rotation.RotationAroundY(angle);
    newTransform *= rotation;

    parent->SetTransformation(newTransform);
}

void Geometry::RotateZ(float angle)
{
    Spatial* parent = GetSpatialParent();

    Matrix4x4 newTransform = parent->GetTransformation();
    Matrix4x4 rotation;
    rotation.RotationAroundZ(angle);
    newTransform *= rotation;

    parent->SetTransformation(newTransform);
}

void Geometry::Scale(float x, float y, float z)
{
    Spatial* parent = GetSpatialParent();

    Matrix4x4 newTransform = parent->GetTransformation();
    Matrix4x4 scale(   x, 0.0f, 0.0f, 0.0f,
                    0.0f,    y, 0.0f, 0.0f,
                    0.0f, 0.0f,    z, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
    newTransform *= scale;

    parent->SetTransformation(newTransform);
}

void Geometry::Scale(const Vector3& scale)
{
    Scale(scale.x, scale.y, scale.z);
}

Geometry::nodeType_t Geometry::GetType() const
{
    return GEOMETRY;
}

Spatial* Geometry::GetSpatialParent()
{
    Node* parent = parent_;
    if (parent->GetType() == SPATIAL) {
        return static_cast<Spatial*>(parent);
    }

    SceneGraph& graph = Renderer::GetInstance()->GetSceneGraph();
    Spatial* newSpatial = graph.CreateSpatialNode(name_ + "_Transformation");
   
    parent_->RemoveChild(this);
    parent_->AddChild(newSpatial);
    newSpatial->AddChild(this);

    return newSpatial;
}
}
