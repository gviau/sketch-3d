#ifndef SKETCH_3D_LIGHT_H
#define SKETCH_3D_LIGHT_H

#include "math/Vector3.h"
#include "math/Vector4.h"

namespace Sketch3D
{
enum LightType_t
{
    Directionnal,
    Point,
    SpotLight,

    NumLights
};

/**
 * @class Light
 * The Light class represents a light object, which defines properties such as its type, position, direction and color.
 */
class Light
{
public:
    void            SetLightType(LightType_t lightType) { m_LightType = lightType; }
    void            SetPosition(const Vector3& position) { m_Position = position; }
    void            SetDirection(const Vector3& direction) { m_Direction = direction; }
    void            SetColor(const Vector4& color) { m_Color = color; }

    LightType_t     GetLightType() const { return m_LightType; }
    const Vector3&  GetPosition() const { return m_Position; }
    const Vector3&  GetDirection() const { return m_Direction; }
    const Vector4&  GetColor() const { return m_Color; }

private:
    LightType_t     m_LightType;

    Vector3         m_Position;
    Vector3         m_Direction;
    Vector4         m_Color;
};
}

#endif