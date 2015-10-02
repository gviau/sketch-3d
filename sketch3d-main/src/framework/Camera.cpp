#include "framework/Camera.h"

namespace Sketch3D {
Camera::Camera()
    : m_ConstrainPlane(ConstrainPlane_t::XZ_PLANE)
    , m_NeedViewMatrixUpdate(true)
{
}

void Camera::LookAt(const Vector3& position, const Vector3& point, const Vector3& up)
{
    m_Position = position;
    Vector3 direction = (point - m_Position).Normalized();

    Vector3 right = direction.Cross(up).Normalized();
    Vector3 newUp = right.Cross(direction).Normalized();

    m_Orientation.MakeFromAxes(right, newUp, direction);

    m_NeedViewMatrixUpdate = true;
}

void Camera::Walk(float units)
{
    if (m_ConstrainPlane != ConstrainPlane_t::YZ_PLANE)
    {
        Vector3 right = m_Orientation.GetXAxis();

        if (m_ConstrainPlane == ConstrainPlane_t::FREE_CAMERA)
        {
            m_Position += right * units;
        }
        else
        {
            Vector3 projectedRightOnPlane = Vector3::RIGHT.Dot(right) * Vector3::RIGHT;
            m_Position += projectedRightOnPlane * units;
        }

        m_NeedViewMatrixUpdate = true;
    }
}

void Camera::Fly(float units)
{
    if (m_ConstrainPlane != ConstrainPlane_t::XZ_PLANE)
    {
        Vector3 up = m_Orientation.GetYAxis();

        if (m_ConstrainPlane == ConstrainPlane_t::FREE_CAMERA)
        {
            m_Position += up * units;
        }
        else
        {
            Vector3 projectedUpOnPlane = Vector3::UP.Dot(up) * Vector3::UP;
            m_Position += projectedUpOnPlane * units;
        }

        m_NeedViewMatrixUpdate = true;
    }
}

void Camera::Strafe(float units)
{
    if (m_ConstrainPlane != ConstrainPlane_t::XY_PLANE)
    {
        Vector3 look = m_Orientation.GetZAxis();

        if (m_ConstrainPlane == ConstrainPlane_t::FREE_CAMERA)
        {
            m_Position += look * units;
        }
        else
        {
            Vector3 projectedLookOnPlane = Vector3::LOOK.Dot(look) * Vector3::LOOK;
            m_Position += projectedLookOnPlane * units;
        }

        m_NeedViewMatrixUpdate = true;
    }
}

void Camera::Pitch(float radianAngle)
{
    Quaternion pitchRotation;
    pitchRotation.MakeFromAngleAxis(radianAngle, Vector3::RIGHT);

    m_Orientation *= pitchRotation;
    m_Orientation.Normalize();

    m_NeedViewMatrixUpdate = true;
}

void Camera::Yaw(float radianAngle)
{
    Quaternion yawRotation;
    yawRotation.MakeFromAngleAxis(radianAngle, Vector3::UP);

    m_Orientation *= yawRotation;
    m_Orientation.Normalize();

    m_NeedViewMatrixUpdate = true;
}

void Camera::Roll(float radianAngle)
{
    Quaternion rollRotation;
    rollRotation.MakeFromAngleAxis(radianAngle, Vector3::LOOK);

    m_Orientation *= rollRotation;
    m_Orientation.Normalize();

    m_NeedViewMatrixUpdate = true;
}

void Camera::SetConstrainPlane(ConstrainPlane_t constrainPlane)
{
    m_ConstrainPlane = constrainPlane;
}

void Camera::SetPosition(const Vector3& position)
{
    m_Position = position;
    m_NeedViewMatrixUpdate = true;
}

void Camera::SetOrientation(const Quaternion& orientation)
{
    m_Orientation = orientation;
    m_NeedViewMatrixUpdate = true;
}

const Matrix4x4& Camera::GetViewMatrix()
{
    if (m_NeedViewMatrixUpdate)
    {
        Vector3 right = m_Orientation.GetXAxis();
        Vector3 up = m_Orientation.GetYAxis();
        Vector3 look = m_Orientation.GetZAxis();

        m_ViewMatrix[0][0] = right.x;
        m_ViewMatrix[1][0] = right.y;
        m_ViewMatrix[2][0] = right.z;

        m_ViewMatrix[0][1] = up.x;
        m_ViewMatrix[1][1] = up.y;
        m_ViewMatrix[2][1] = up.z;

        m_ViewMatrix[0][2] = look.x;
        m_ViewMatrix[1][2] = look.y;
        m_ViewMatrix[2][2] = look.z;

        m_ViewMatrix[0][3] = -m_Position.Dot(right);
        m_ViewMatrix[1][3] = -m_Position.Dot(up);
        m_ViewMatrix[2][3] = -m_Position.Dot(look);

        m_NeedViewMatrixUpdate = false;
    }

    return m_ViewMatrix;
}

const Vector3& Camera::GetPosition() const
{
    return m_Position;
}

const Quaternion& Camera::GetOrientation() const
{
    return m_Orientation;
}

}