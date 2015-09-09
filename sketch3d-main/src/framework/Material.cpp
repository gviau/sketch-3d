#include "framework/Material.h"

#include "render/RenderDevice.h"
#include "render/SamplerState.h"
#include "render/Shader.h"
#include "render/Texture.h"

namespace Sketch3D {
Material::Material()
    : m_SpecularPower(1.0f)
{
}

bool Material::Apply(const shared_ptr<RenderDevice>& renderDevice) const
{
    // TEMP
    // The material would eventually create its own shader depending of the material or use an uber shader
    RenderDevice* device = renderDevice.get();
    if (device == nullptr)
    {
        return false;
    }

    device->SetFragmentShaderSamplerState(m_DiffuseSamplerState, 0);
    device->SetFragmentShaderTexture(m_DiffuseTexture, 0);

    return true;
}

void Material::SetAmbientColor(const Vector3& color)
{
    m_AmbientColor = color;
}

void Material::SetDiffuseColor(const Vector3& color)
{
    m_DiffuseColor = color;
}

void Material::SetSpecularColor(const Vector3& color)
{
    m_SpecularColor = color;
}

void Material::SetSpecularPower(float power)
{
    m_SpecularPower = power;
}

void Material::SetAmbientTexture(const shared_ptr<Texture2D>& texture)
{
    m_AmbientTexture = texture;
}

void Material::SetAmbientSamplerState(const shared_ptr<SamplerState>& samplerState)
{
    m_AmbientSamplerState = samplerState;
}

void Material::SetDiffuseTexture(const shared_ptr<Texture2D>& texture)
{
    m_DiffuseTexture = texture;
}

void Material::SetDiffuseSamplerState(const shared_ptr<SamplerState>& samplerState)
{
    m_DiffuseSamplerState = samplerState;
}

void Material::SetSpecularTexture(const shared_ptr<Texture2D>& texture)
{
    m_SpecularTexture = texture;
}

void Material::SetSpecularSamplerState(const shared_ptr<SamplerState>& samplerState)
{
    m_SpecularSamplerState = samplerState;
}

void Material::SetNormalMapTexture(const shared_ptr<Texture2D>& texture)
{
    m_NormalMapTexture = texture;
}

void Material::SetNormalMapSamplerState(const shared_ptr<SamplerState>& samplerState)
{
    m_NormalMapSamplerState = samplerState;
}

const Vector3& Material::GetAmbientColor() const
{
    return m_AmbientColor;
}

const Vector3& Material::GetDiffuseColor() const
{
    return m_DiffuseColor;
}

const Vector3& Material::GetSpecularColor() const
{
    return m_SpecularColor;
}

float Material::GetSpecularPower() const
{
    return m_SpecularPower;
}

const shared_ptr<Texture2D>& Material::GetAmbientTexture() const
{
    return m_AmbientTexture;
}

const shared_ptr<SamplerState>& Material::GetAmbientSamplerState() const
{
    return m_AmbientSamplerState;
}

const shared_ptr<Texture2D>& Material::GetDiffuseTexture() const
{
    return m_DiffuseTexture;
}

const shared_ptr<SamplerState>& Material::GetDiffuseSamplerState() const
{
    return m_DiffuseSamplerState;
}

const shared_ptr<Texture2D>& Material::GetSpecularTexture() const
{
    return m_SpecularTexture;
}

const shared_ptr<SamplerState>& Material::GetSpecularSamplerState() const
{
    return m_SpecularSamplerState;
}

const shared_ptr<Texture2D>& Material::GetNormalMapTexture() const
{
    return m_NormalMapTexture;
}

const shared_ptr<SamplerState>& Material::GetNormalMapSamplerState() const
{
    return m_NormalMapSamplerState;
}

}