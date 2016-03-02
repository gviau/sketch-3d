#include "framework/Light.h"

namespace Sketch3D
{
Light::Light()
    : m_LightType(LightType_t::Point)
    , m_Position(Vector3(0.0f, 0.0f, 0.0f))
    , m_Direction(Vector3::LOOK)
    , m_Color(Vector4::ONE)
{
}

void Light::GetLightConstants(Vector4& position, Vector4& direction, Vector4& color) const
{
    position = m_Position;
    if (m_LightType == LightType_t::Directionnal)
    {
        position.w = 0.0f;
    }

    direction = m_Direction;
    color = m_Color;
}

}