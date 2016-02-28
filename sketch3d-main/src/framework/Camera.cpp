#include "framework/Camera.h"

namespace Sketch3D {
Camera::Camera()
    : m_ConstrainPlane(ConstrainPlane_t::XZ_PLANE)
    , m_NeedViewMatrixUpdate(true)
    , m_IsRightHanded(true)
{
}

void Camera::LookAtRightHanded(const Vector3& position, const Vector3& point, const Vector3& up)
{
    m_Position = position;
    m_Look = (m_Position - point).Normalized();

    m_Right = up.Cross(m_Look).Normalized();
    m_Up = m_Look.Cross(m_Right);

    m_NeedViewMatrixUpdate = true;
    m_IsRightHanded = true;
}

void Camera::LookAtLeftHanded(const Vector3& position, const Vector3& point, const Vector3& up)
{
    m_Position = position;
    m_Look = (point - m_Position).Normalized();

    m_Right = up.Cross(m_Look).Normalized();
    m_Up = m_Look.Cross(m_Right);

    m_NeedViewMatrixUpdate = true;
    m_IsRightHanded = false;
}

void Camera::Walk(float units)
{
    if (m_ConstrainPlane != ConstrainPlane_t::YZ_PLANE)
    {
        if (m_ConstrainPlane == ConstrainPlane_t::FREE_CAMERA)
        {
            m_Position += m_Right * units;
        }
        else
        {
            Vector3 projectedRightOnPlane = Vector3::RIGHT.Dot(m_Right) * Vector3::RIGHT;
            m_Position += projectedRightOnPlane * units;
        }

        m_NeedViewMatrixUpdate = true;
    }
}

void Camera::Fly(float units)
{
    if (m_ConstrainPlane != ConstrainPlane_t::XZ_PLANE)
    {
        if (m_ConstrainPlane == ConstrainPlane_t::FREE_CAMERA)
        {
            m_Position += m_Up * units;
        }
        else
        {
            Vector3 projectedUpOnPlane = Vector3::UP.Dot(m_Up) * Vector3::UP;
            m_Position += projectedUpOnPlane * units;
        }

        m_NeedViewMatrixUpdate = true;
    }
}

void Camera::Strafe(float units)
{
    if (m_ConstrainPlane != ConstrainPlane_t::XY_PLANE)
    {
        if (m_ConstrainPlane == ConstrainPlane_t::FREE_CAMERA)
        {
            m_Position += m_Look * units;
        }
        else
        {
            Vector3 projectedLookOnPlane = Vector3::LOOK.Dot(m_Look) * Vector3::LOOK;
            m_Position += projectedLookOnPlane * units;
        }

        m_NeedViewMatrixUpdate = true;
    }
}

void Camera::Pitch(float radianAngle)
{
    m_Look = (m_Look * cosf(radianAngle) + m_Up * sinf(radianAngle)).Normalized();
    m_Up = m_Right.Cross(m_Look);

    m_NeedViewMatrixUpdate = true;
}

void Camera::Yaw(float radianAngle)
{
    m_Look = (m_Look * cosf(radianAngle) - m_Right * sinf(radianAngle)).Normalized();
    m_Right = m_Look.Cross(m_Up);

    m_NeedViewMatrixUpdate = true;
}

void Camera::Roll(float radianAngle)
{
    m_Right = (m_Right * cosf(radianAngle) + m_Up * sinf(radianAngle)).Normalized();
    m_Up = m_Look.Cross(m_Right);

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

void Camera::SetOrientation(const Vector3& right, const Vector3& up, const Vector3& look)
{
    m_Right = right;
    m_Up = up;
    m_Look = look;
    m_NeedViewMatrixUpdate = true;
}

const Matrix4x4& Camera::GetViewMatrix()
{
    if (m_NeedViewMatrixUpdate)
    {
        m_ViewMatrix[0][0] = m_Right.x;
        m_ViewMatrix[0][1] = m_Right.y;
        m_ViewMatrix[0][2] = m_Right.z;

        m_ViewMatrix[1][0] = m_Up.x;
        m_ViewMatrix[1][1] = m_Up.y;
        m_ViewMatrix[1][2] = m_Up.z;

        m_ViewMatrix[2][0] = m_Look.x;
        m_ViewMatrix[2][1] = m_Look.y;
        m_ViewMatrix[2][2] = m_Look.z;

        if (m_IsRightHanded)
        {
            m_ViewMatrix[0][3] = -m_Position.Dot(m_Right);
            m_ViewMatrix[1][3] = -m_Position.Dot(m_Up);
            m_ViewMatrix[2][3] = -m_Position.Dot(m_Look);
        }
        else
        {
            m_ViewMatrix[0][3] = -m_Position.Dot(m_Right);
            m_ViewMatrix[1][3] = -m_Position.Dot(m_Up);
            m_ViewMatrix[2][3] = -m_Position.Dot(m_Look);
        }

        m_NeedViewMatrixUpdate = false;
    }

    return m_ViewMatrix;
}

const Vector3& Camera::GetPosition() const
{
    return m_Position;
}

const Vector3& Camera::GetRight() const
{
    return m_Right;
}

const Vector3& Camera::GetUp() const
{
    return m_Up;
}

const Vector3& Camera::GetLook() const
{
    return m_Look;
}

}