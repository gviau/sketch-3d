#include "framework/VisualNode.h"

#include "framework/Material.h"
#include "framework/Mesh.h"
#include "framework/SubMesh.h"

namespace Sketch3D
{
VisualNode::VisualNode()
    : m_Parent(nullptr)
    , m_NeedsTransformationMatrixUpdate(false)
    , m_Scale(1.0f, 1.0f, 1.0f)
    , m_AmbientColor(0.0f, 0.0f, 0.0f)
    , m_DiffuseColor(1.0f, 1.0f, 1.0f)
    , m_SpecularColor(1.0f, 1.0f, 1.0f)
    , m_SpecularPower(0.0f)
{
}

void VisualNode::AddChild(const shared_ptr<VisualNode>& childNode)
{
    childNode->m_Parent = this;
    m_Children.push_back(childNode);
}

void VisualNode::SetPosition(const Vector3& position)
{
    m_Position = position;
    NotifyParentForTransformationMatrixUpdate_r();
}

void VisualNode::SetPosition(float x, float y, float z)
{
    m_Position.x = x;
    m_Position.y = y;
    m_Position.z = z;
    NotifyParentForTransformationMatrixUpdate_r();
}

void VisualNode::SetOrientation(const Quaternion& orientation)
{
    m_Orientation = orientation;
    NotifyParentForTransformationMatrixUpdate_r();
}

void VisualNode::SetOrientation(float pitchInRadians, float yawInRadians, float rollInRadians)
{
    m_Orientation.MakeFromEulerAngles(pitchInRadians, yawInRadians, rollInRadians);
    NotifyParentForTransformationMatrixUpdate_r();
}

void VisualNode::SetScale(const Vector3& scale)
{
    m_Scale = scale;
    NotifyParentForTransformationMatrixUpdate_r();
}

void VisualNode::SetScale(float x, float y, float z)
{
    m_Scale.x = x;
    m_Scale.y = y;
    m_Scale.z = z;
    NotifyParentForTransformationMatrixUpdate_r();
}

void VisualNode::Translate(const Vector3& translation)
{
    m_Position += translation;
    NotifyParentForTransformationMatrixUpdate_r();
}

void VisualNode::Translate(float x, float y, float z)
{
    m_Position += Vector3(x, y, z);
    NotifyParentForTransformationMatrixUpdate_r();
}

void VisualNode::RotateAroundAxis(const Vector3& axis, float angleInRadians)
{
    Quaternion rotation;
    rotation.MakeFromAngleAxis(angleInRadians, axis);
    rotation.Normalize();

    m_Orientation = rotation * m_Orientation;

    NotifyParentForTransformationMatrixUpdate_r();
}

void VisualNode::Pitch(float angleInRadians)
{
    RotateAroundAxis(Vector3::RIGHT, angleInRadians);
}

void VisualNode::Yaw(float angleInRadians)
{
    RotateAroundAxis(Vector3::UP, angleInRadians);
}

void VisualNode::Roll(float angleInRadians)
{
    RotateAroundAxis(Vector3::LOOK, angleInRadians);
}

void VisualNode::Scale(const Vector3& scaling)
{
    m_Scale += scaling;
    NotifyParentForTransformationMatrixUpdate_r();
}

void VisualNode::Scale(float x, float y, float z)
{
    m_Scale += Vector3(x, y, z);
    NotifyParentForTransformationMatrixUpdate_r();
}

const Matrix4x4& VisualNode::GetModelMatrix()
{
    if (m_NeedsTransformationMatrixUpdate)
    {
        Matrix4x4 parentMatrix = Matrix4x4::IDENTITY;
        if (m_Parent != nullptr)
        {
            parentMatrix = m_Parent->GetModelMatrix();
        }

        m_ModelMatrix = Matrix4x4::IDENTITY;
        m_ModelMatrix[0][0] = m_Scale.x;
        m_ModelMatrix[1][1] = m_Scale.y;
        m_ModelMatrix[2][2] = m_Scale.z;

        Matrix4x4 rotation;
        m_Orientation.ToRotationMatrix(rotation);
        m_ModelMatrix = rotation * m_ModelMatrix;

        m_ModelMatrix[0][3] = m_Position.x;
        m_ModelMatrix[1][3] = m_Position.y;
        m_ModelMatrix[2][3] = m_Position.z;

        m_ModelMatrix = parentMatrix * m_ModelMatrix;

        m_NeedsTransformationMatrixUpdate = false;
    }

    return m_ModelMatrix;
}

void VisualNode::NotifyParentForTransformationMatrixUpdate_r()
{
    m_NeedsTransformationMatrixUpdate = true;

    if (m_Parent != nullptr)
    {
        m_Parent->NotifyParentForTransformationMatrixUpdate_r();
    }
}
}