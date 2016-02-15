#include "framework/MaterialCodeGenerator.h"

#include "render/ConstantBuffers.h"

namespace Sketch3D
{
void MaterialCodeGenerator::WriteShader(VertexFormatType_t vertexFormatType, bool usesAmbientTexture, bool usesDiffuseTexture, bool usesSpecularTexture,
                                        bool usesNormalTexture, string& vertexShaderCode, string& fragmentShaderCode)
{
    m_HasColors = false;
    m_HasNormals = false;
    m_HasUVs = false;
    m_HasTangents = false;
    m_HasBones = false;
    m_Has2UVs = false;

    m_UsesAmbientTexture = usesAmbientTexture;
    m_UsesDiffuseTexture = usesDiffuseTexture;
    m_UsesSpecularTexture = usesSpecularTexture;
    m_UsesNormalTexture = usesNormalTexture;

    // Vertex shader
    WriteVertexShaderConstantBuffers(vertexShaderCode);

    vertexShaderCode += "struct " + GetVertexShaderInputStructureName() + " {\n";
    WriteVertexShaderInputs(vertexFormatType, vertexShaderCode);
    vertexShaderCode += "};\n";

    vertexShaderCode += "struct " + GetVertexShaderOutputStructureName() + " {\n";
    WriteVertexShaderOutputs(vertexShaderCode);
    vertexShaderCode += "};\n";

    vertexShaderCode += GetVertexShaderOutputStructureName() + " main(" + GetVertexShaderInputStructureName() + " " + GetVertexShaderInputStructureVariableName() + ") {\n";
    WriteVertexShaderMainBody(vertexShaderCode);
    vertexShaderCode += "};\n";

    // Fragment shader
    WriteFragmentShaderConstantBuffers(fragmentShaderCode);

    fragmentShaderCode += "struct " + GetFragmentShaderInputStructureName() + " {\n";
    WriteFragmentShaderInputs(fragmentShaderCode);
    fragmentShaderCode += "};\n";

    fragmentShaderCode += "struct " + GetFragmentShaderOutputStructureName() + " {\n";
    WriteFragmentShaderOutputs(fragmentShaderCode);
    fragmentShaderCode += "};\n";

    fragmentShaderCode += GetFragmentShaderOutputStructureName() + " main(" + GetFragmentShaderInputStructureName() + " " + GetFragmentShaderInputStructureVariableName() + ") {\n";
    WriteFragmentShaderMainBody(fragmentShaderCode);
    fragmentShaderCode += "};\n";
}

void MaterialCodeGenerator::WriteVertexShaderConstantBuffers(string& shaderCode)
{
    shaderCode +=
        "cbuffer DrawConstants_t : register(b0) {\n"
        "    float4x4 modelMatrix;\n"
        "    float4x4 modelViewProjectionMatrix;\n"
        "    float4x4 viewProjectionMatrix;\n"
        "    float4x4 modelViewMatrix;\n"
        "    float4x4 inverseTransposeModelViewMatrix;\n"
        "};\n";
}

void MaterialCodeGenerator::WriteVertexShaderInputs(VertexFormatType_t vertexFormatType, string& shaderCode)
{
    shaderCode += "float3 in_vertex : POSITION;\n";

    switch (vertexFormatType)
    {
        case VertexFormatType_t::Pos_Color:
            shaderCode += "float3 in_color : COLOR;\n";
            m_HasColors = true;
            break;

        case VertexFormatType_t::Pos_UV:
            shaderCode += "float2 in_uv : TEXCOORD;\n";
            m_HasUVs = true;
            break;

        case VertexFormatType_t::Pos_Normal:
            shaderCode += "float3 in_normal : NORMAL;\n";
            m_HasNormals = true;
            break;

        case VertexFormatType_t::Pos_UV_Normal:
            shaderCode += "float2 in_uv : TEXCOORD; float3 in_normal : NORMAL;\n";
            m_HasNormals = true;
            break;

        case VertexFormatType_t::Pos_UV_Normal_Tangent:
            shaderCode += "float2 in_uv : TEXCOORD; float3 in_normal : NORMAL; float3 in_tangent : TANGENT;\n";
            m_HasNormals = true;
            m_HasTangents = true;
            break;

        case VertexFormatType_t::Pos_UV_Normal_4_Bones:
            shaderCode += "float2 in_uv : TEXCOORD; float3 in_normal : NORMAL; float4 in_bones : BLENDINDICES; float4 in_weights : BLENDWEIGHT;\n";
            m_HasUVs = true;
            m_HasNormals = true;
            m_HasBones = true;
            break;

        case VertexFormatType_t::Pos_UV_Normal_Tangent_4_Bones:
            shaderCode += "float2 in_uv : TEXCOORD; float3 in_normal : NORMAL; float3 in_tangent : TANGENT; float4 in_bones : BLENDINDICES; float4 in_weights : BLENDWEIGHT;\n";
            m_HasUVs = true;
            m_HasNormals = true;
            m_HasTangents = true;
            m_HasBones = true;
            break;

        case VertexFormatType_t::Pos_UV_4_Bones:
            shaderCode += "float2 in_uv : TEXCOORD; float4 in_bones : BLENDINDICES; float4 in_weights : BLENDWEIGHT;\n";
            m_HasUVs = true;
            m_HasBones = true;
            break;

        case VertexFormatType_t::Pos_Normal_4_Bones:
            shaderCode += "float3 in_normal : NORMAL; float4 in_bones : BLENDINDICES; float4 in_weights : BLENDWEIGHT;\n";
            m_HasNormals = true;
            m_HasBones = true;
            break;

        case VertexFormatType_t::Pos_2_UV:
            shaderCode += "float2 in_uv1 : TEXCOORD0; float2 in_uv2 : TEXCOORD1;\n";
            m_Has2UVs = true;
            break;

        case VertexFormatType_t::Pos_2_UV_Normal:
            shaderCode += "float2 in_uv1 : TEXCOORD0; float2 in_uv2 : TEXCOORD1; float3 in_normal : NORMAL;\n";
            m_Has2UVs = true;
            m_HasNormals = true;
            break;

        case VertexFormatType_t::Pos_2_UV_Normal_Tangent:
            shaderCode += "float2 in_uv1 : TEXCOORD0; float2 in_uv2 : TEXCOORD1; float3 in_normal : NORMAL; float3 in_tangent : TANGENT;\n";
            m_HasUVs = true;
            m_HasNormals = true;
            m_HasTangents = true;
            break;

        case VertexFormatType_t::Pos_2_UV_Normal_4_Bones:
            shaderCode += "float2 in_uv1 : TEXCOORD0; float2 in_uv2 : TEXCOORD1; float3 in_normal : NORMAL; float4 in_bones : BLENDINDICES; float4 in_weights : BLENDWEIGHT;\n";
            m_Has2UVs = true;
            m_HasNormals = true;
            m_HasBones = true;
            break;

        case VertexFormatType_t::Pos_2_UV_Normal_Tangent_4_Bones:
            shaderCode += "float2 in_uv1 : TEXCOORD0; float2 in_uv2 : TEXCOORD1; float3 in_normal : NORMAL; float3 in_tangent : TANGENT; float4 in_bones : BLENDINDICES; float4 in_weights : BLENDWEIGHT;\n";
            m_Has2UVs = true;
            m_HasNormals = true;
            m_HasTangents = true;
            m_HasBones = true;
            break;

        case VertexFormatType_t::Pos_2_UV_4_Bones:
            shaderCode += "float2 in_uv1 : TEXCOORD0; float2 in_uv2 : TEXCOORD1; float4 in_bones : BLENDINDICES; float4 in_weights : BLENDWEIGHT;\n";
            m_Has2UVs = true;
            m_HasBones = true;
            break;
    }
}

void MaterialCodeGenerator::WriteVertexShaderOutputs(string& shaderCode)
{
    shaderCode += "float4 position : SV_POSITION;\n";

    if (m_HasColors)
    {
        shaderCode += "float4 color: COLOR;\n";
    }

    unsigned int semanticIndex = 0;
    if (m_HasNormals)
    {
        shaderCode += "float3 normal : TEXCOORD" + to_string(semanticIndex++) +";\n";
    }

    if (m_Has2UVs)
    {
        shaderCode += "float2 uv1 : TEXCOORD" + to_string(semanticIndex++) + ";\n";
        shaderCode += "float2 uv2 : TEXCOORD" + to_string(semanticIndex++) + ";\n";
    }
    else if (m_HasUVs)
    {
        shaderCode += "float2 uv : TEXCOORD" + to_string(semanticIndex++) + ";\n";
    }

    shaderCode += "float3 eyePosition : TEXCOORD" + to_string(semanticIndex++) + ";\n";
}

void MaterialCodeGenerator::WriteFragmentShaderConstantBuffers(string& shaderCode)
{
    shaderCode += "SamplerState samplerState : register(s0);\n";

    if (m_UsesAmbientTexture)
    {
        shaderCode += "Texture2D ambientTexture : register(t0);\n";
    }

    if (m_UsesDiffuseTexture)
    {
        shaderCode += "Texture2D diffuseTexture : register(t1);\n";
    }

    if (m_UsesSpecularTexture)
    {
        shaderCode += "Texture2D specularTexture : register(t2);\n";
    }

    if (m_UsesNormalTexture)
    {
        shaderCode += "Texture2D normalTexture : register(t3);\n";
    }

    shaderCode +=
        "cbuffer PassConstants_t : register(b0) {\n"
        "float4x4 projectionMatrix;\n"
        "float4x4 viewMatrix;\n"
        "};\n";

    shaderCode +=
        "cbuffer MaterialConstants_t : register(b1) {\n"
        "float4 ambientColor;\n"
        "float4 diffuseColor;\n"
        "float4 specularColorAndPower;\n"
        "};\n";

    string numLightsString = to_string(Sketch3D::MAX_NUM_LIGHTS);
    shaderCode +=
        "cbuffer LightConstants_t : register(b2) {\n"
        "int numLights;\n"
        "float4 lightPositions[" + numLightsString + "];\n"
        "float4 lightDirections[" + numLightsString + "];\n"
        "float4 lightColors[" + numLightsString + "];\n";
}
     
void MaterialCodeGenerator::WriteFragmentShaderInputs(string& shaderCode)
{
    WriteVertexShaderOutputs(shaderCode);
}

void MaterialCodeGenerator::WriteFragmentShaderOutputs(string& shaderCode)
{
    shaderCode += "float4 color : SV_TARGET;\n";
}
}