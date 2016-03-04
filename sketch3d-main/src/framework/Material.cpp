#include "framework/Material.h"

#include "framework/MaterialCodeGenerator.h"

#include "render/Buffer.h"
#include "render/ConstantBuffers.h"
#include "render/HardwareResourceCreator.h"
#include "render/RenderDevice.h"
#include "render/SamplerState.h"
#include "render/Shader.h"
#include "render/Texture.h"

namespace Sketch3D {
Material::Material(const shared_ptr<RenderDevice>& renderDevice)
    : m_RenderDevice(renderDevice)
    , m_AmbientColor(Vector3::ZERO)
    , m_DiffuseColor(1.0f, 1.0f, 1.0f)
    , m_SpecularColor(1.0f, 1.0f, 1.0f)
    , m_SpecularPower(0.0f)
    , m_MaterialConstantsBufferSlot(0)
{
}

void Material::Initialize(VertexFormatType_t vertexFormatType, MaterialCodeGenerator* materialCodeGenerator)
{
    MaterialConstants_t materialConstants;
    materialConstants.ambientColor = m_AmbientColor;
    materialConstants.diffuseColor = m_DiffuseColor;
    materialConstants.specularColorAndPower = m_SpecularColor;
    materialConstants.specularColorAndPower.w = m_SpecularPower;
 
    m_MaterialConstants = m_RenderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();
    m_MaterialConstants->Initialize(&materialConstants, false, false, sizeof(MaterialConstants_t));

    if (materialCodeGenerator != nullptr)
    {
        string vertexShaderCode;
        string fragmentShaderCode;
        bool usesAmbientTexture = (m_AmbientTexture.get() != nullptr);
        bool usesDiffuseTexture = (m_DiffuseTexture.get() != nullptr);
        bool usesSpecularTexture = (m_SpecularTexture.get() != nullptr);
        bool usesNormalTexture = (m_NormalMapTexture.get() != nullptr);

        materialCodeGenerator->WriteShader(vertexFormatType, usesAmbientTexture, usesDiffuseTexture, usesSpecularTexture, usesNormalTexture,
                                           vertexShaderCode, fragmentShaderCode);

        if (m_VertexShader.get() == nullptr)
        {
            m_VertexShader = m_RenderDevice->GetHardwareResourceCreator()->CreateVertexShader();
        }

        if (m_FragmentShader.get() == nullptr)
        {
            m_FragmentShader = m_RenderDevice->GetHardwareResourceCreator()->CreateFragmentShader();
        }

        m_VertexShader->InitializeFromSource(vertexShaderCode);
        m_FragmentShader->InitializeFromSource(fragmentShaderCode);
    }
}

bool Material::Apply() const
{
    RenderDevice* renderDevice = m_RenderDevice.get();
    if (renderDevice == nullptr)
    {
        return false;
    }

    // Set the shaders to use
    renderDevice->SetVertexShader(m_VertexShader);
    renderDevice->SetFragmentShader(m_FragmentShader);

    // Set the constant buffers for the shaders
    for (size_t i = 0; i < m_VertexShaderConstantBuffers.size(); i++)
    {
        renderDevice->SetVertexShaderConstantBuffer(m_VertexShaderConstantBuffers[i], i);
    }

    renderDevice->SetFragmentShaderConstantBuffer(m_MaterialConstants, m_MaterialConstantsBufferSlot);
    for (size_t i = 0; i < m_FragmentShaderConstantBuffers.size(); i++)
    {
        renderDevice->SetFragmentShaderConstantBuffer(m_FragmentShaderConstantBuffers[i], i + 1);
    }

    // Set the textures and sampler states for the shaders
    renderDevice->SetFragmentShaderSamplerState(m_DiffuseSamplerState, 0);

    if (m_AmbientTexture.get() != nullptr)
    {
        renderDevice->SetFragmentShaderTexture(m_AmbientTexture, 0);
    }

    if (m_DiffuseTexture.get() != nullptr)
    {
        renderDevice->SetFragmentShaderTexture(m_DiffuseTexture, 1);
    }

    if (m_SpecularTexture.get() != nullptr)
    {
        renderDevice->SetFragmentShaderTexture(m_SpecularTexture, 2);
    }

    if (m_NormalMapTexture.get() != nullptr)
    {
        renderDevice->SetFragmentShaderTexture(m_NormalMapTexture, 3);
    }

    return true;
}

void Material::SetVertexShader(const shared_ptr<VertexShader>& vertexShader)
{
    m_VertexShader = vertexShader;
}

void Material::SetFragmentShader(const shared_ptr<FragmentShader>& fragmentShader)
{
    m_FragmentShader = fragmentShader;
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

const shared_ptr<VertexShader>& Material::GetVertexShader() const
{
    return m_VertexShader;
}

const shared_ptr<FragmentShader>& Material::GetFragmentShader() const
{
    return m_FragmentShader;
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