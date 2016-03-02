#include "framework/MaterialCodeGenerator.h"

#include "render/ConstantBuffers.h"

#include <typeinfo>

namespace Sketch3D
{
void MaterialCodeGenerator::WriteShader(VertexFormatType_t vertexFormatType, bool usesAmbientTexture, bool usesDiffuseTexture, bool usesSpecularTexture,
                                        bool usesNormalTexture, string& vertexShaderCode, string& fragmentShaderCode)
{
    m_HasColors = VertexFormatTypeContainsColor(vertexFormatType);
    m_HasNormals = VertexFormatTypeContainsNormals(vertexFormatType);
    m_HasUVs = VertexFormatTypeContainsUV(vertexFormatType);
    m_HasTangents = VertexFormatTypeContainsTangents(vertexFormatType);
    m_HasBones = VertexFormatTypeContainsBones(vertexFormatType);
    m_Has2UVs = VertexFormatTypeContains2UVs(vertexFormatType);

    m_UsesAmbientTexture = usesAmbientTexture;
    m_UsesDiffuseTexture = usesDiffuseTexture;
    m_UsesSpecularTexture = usesSpecularTexture;
    m_UsesNormalTexture = usesNormalTexture;

    // Vertex shader
    WriteVertexShaderConstantBuffers(vertexShaderCode);

    vertexShaderCode += "struct " + GetVertexShaderInputStructureName() + " {\n";
    WriteVertexShaderInputs(vertexFormatType, vertexShaderCode);
    vertexShaderCode += "};\n\n";

    vertexShaderCode += "struct " + GetVertexShaderOutputStructureName() + " {\n";
    WriteVertexShaderOutputs(vertexShaderCode);
    vertexShaderCode += "};\n\n";

    vertexShaderCode += GetVertexShaderOutputStructureName() + " main(" + GetVertexShaderInputStructureName() + " " + GetVertexShaderInputStructureVariableName() + ") {\n";
    WriteVertexShaderMainBody(vertexShaderCode);
    vertexShaderCode += "}\n";

    // Fragment shader
    WriteFragmentShaderConstantBuffers(fragmentShaderCode);

    fragmentShaderCode += "struct " + GetFragmentShaderInputStructureName() + " {\n";
    WriteFragmentShaderInputs(fragmentShaderCode);
    fragmentShaderCode += "};\n\n";

    fragmentShaderCode += "struct " + GetFragmentShaderOutputStructureName() + " {\n";
    WriteFragmentShaderOutputs(fragmentShaderCode);
    fragmentShaderCode += "};\n\n";

    fragmentShaderCode += GetFragmentShaderOutputStructureName() + " main(" + GetFragmentShaderInputStructureName() + " " + GetFragmentShaderInputStructureVariableName() + ") {\n";
    WriteFragmentShaderMainBody(fragmentShaderCode);
    fragmentShaderCode += "}\n";
}

map<string, size_t> MaterialCodeGenerator::GetVertexShaderConstantBuffersSlots() const
{
    map<string, size_t> constantBufferSlots;

    constantBufferSlots[typeid(PassConstants_t).name()] = 0;
    constantBufferSlots[typeid(DrawConstants_t).name()] = 1;

    return constantBufferSlots;
}

map<string, size_t> MaterialCodeGenerator::GetFragmentShaderConstantBuffersSlots() const
{
    map<string, size_t> constantBufferSlots;

    constantBufferSlots[typeid(PassConstants_t).name()] = 0;
    constantBufferSlots[typeid(LightConstants_t).name()] = 1;

    constantBufferSlots[typeid(MaterialConstants_t).name()] = 0;

    return constantBufferSlots;
}

void MaterialCodeGenerator::WriteVertexShaderConstantBuffers(string& shaderCode)
{
    shaderCode +=
        "cbuffer PassConstants_t : register(b0) {\n"
        "    float4x4 projectionMatrix;\n"
        "    float4x4 viewMatrix;\n"
        "    float4x4 viewProjectionMatrix;\n"
        "};\n";

    shaderCode +=
        "cbuffer DrawConstants_t : register(b1) {\n"
        "    float4x4 modelMatrix;\n"
        "    float4x4 modelViewProjectionMatrix;\n"
        "    float4x4 modelViewMatrix;\n"
        "    float4x4 transposedInverseModelViewMatrix;\n"
        "};\n";
}

void MaterialCodeGenerator::WriteVertexShaderInputs(VertexFormatType_t vertexFormatType, string& shaderCode)
{
    string tab = GetTabs(1);
    shaderCode += tab + "float3 in_vertex : POSITION;\n";

    switch (vertexFormatType)
    {
        case VertexFormatType_t::Pos_Color:
            shaderCode += tab + "float3 in_color : COLOR;\n";
            m_HasColors = true;
            break;

        case VertexFormatType_t::Pos_UV:
            shaderCode += tab + "float2 in_uv : TEXCOORD;\n";
            m_HasUVs = true;
            break;

        case VertexFormatType_t::Pos_Normal:
            shaderCode += tab + "float3 in_normal : NORMAL;\n";
            m_HasNormals = true;
            break;

        case VertexFormatType_t::Pos_UV_Normal:
            shaderCode += tab + "float2 in_uv : TEXCOORD;\n" + tab + "float3 in_normal : NORMAL;\n";
            m_HasNormals = true;
            break;

        case VertexFormatType_t::Pos_UV_Normal_Tangent:
            shaderCode += tab + "float2 in_uv : TEXCOORD;\n" + tab + "float3 in_normal : NORMAL;\n" + tab + "float3 in_tangent : TANGENT;\n";
            m_HasNormals = true;
            m_HasTangents = true;
            break;

        case VertexFormatType_t::Pos_UV_Normal_4_Bones:
            shaderCode += tab + "float2 in_uv : TEXCOORD;\n" + tab + "float3 in_normal : NORMAL;\n" + tab + "float4 in_bones : BLENDINDICES;\n" + tab + "float4 in_weights : BLENDWEIGHT;\n";
            m_HasUVs = true;
            m_HasNormals = true;
            m_HasBones = true;
            break;

        case VertexFormatType_t::Pos_UV_Normal_Tangent_4_Bones:
            shaderCode += tab + "float2 in_uv : TEXCOORD;\n" + tab + "float3 in_normal : NORMAL;\n" + tab + "float3 in_tangent : TANGENT;\n" + tab + "float4 in_bones : BLENDINDICES;\n" + tab + "float4 in_weights : BLENDWEIGHT;\n";
            m_HasUVs = true;
            m_HasNormals = true;
            m_HasTangents = true;
            m_HasBones = true;
            break;

        case VertexFormatType_t::Pos_UV_4_Bones:
            shaderCode += tab + "float2 in_uv : TEXCOORD;\n" + tab + "float4 in_bones : BLENDINDICES;\n" + tab + "float4 in_weights : BLENDWEIGHT;\n";
            m_HasUVs = true;
            m_HasBones = true;
            break;

        case VertexFormatType_t::Pos_Normal_4_Bones:
            shaderCode += tab + "float3 in_normal : NORMAL;\n" + tab + "float4 in_bones : BLENDINDICES;\n" + tab + "float4 in_weights : BLENDWEIGHT;\n";
            m_HasNormals = true;
            m_HasBones = true;
            break;

        case VertexFormatType_t::Pos_2_UV:
            shaderCode += tab + "float2 in_uv1 : TEXCOORD0;\n" + tab + "float2 in_uv2 : TEXCOORD1;\n";
            m_Has2UVs = true;
            break;

        case VertexFormatType_t::Pos_2_UV_Normal:
            shaderCode += tab + "float2 in_uv1 : TEXCOORD0;\n" + tab + "float2 in_uv2 : TEXCOORD1;\n" + tab + "float3 in_normal : NORMAL;\n";
            m_Has2UVs = true;
            m_HasNormals = true;
            break;

        case VertexFormatType_t::Pos_2_UV_Normal_Tangent:
            shaderCode += tab + "float2 in_uv1 : TEXCOORD0;\n" + tab + "float2 in_uv2 : TEXCOORD1;\n" + tab + "float3 in_normal : NORMAL;\n" + tab + "float3 in_tangent : TANGENT;\n";
            m_HasUVs = true;
            m_HasNormals = true;
            m_HasTangents = true;
            break;

        case VertexFormatType_t::Pos_2_UV_Normal_4_Bones:
            shaderCode += tab + "float2 in_uv1 : TEXCOORD0;\n" + tab + "float2 in_uv2 : TEXCOORD1;\n" + tab + "float3 in_normal : NORMAL;\n" + tab + "float4 in_bones : BLENDINDICES;\n" + tab + "float4 in_weights : BLENDWEIGHT;\n";
            m_Has2UVs = true;
            m_HasNormals = true;
            m_HasBones = true;
            break;

        case VertexFormatType_t::Pos_2_UV_Normal_Tangent_4_Bones:
            shaderCode += tab + "float2 in_uv1 : TEXCOORD0;\n" + tab + "float2 in_uv2 : TEXCOORD1;\n" + tab + "float3 in_normal : NORMAL;\n" + tab + "float3 in_tangent : TANGENT;\n" + tab + "float4 in_bones : BLENDINDICES;\n" + tab + "float4 in_weights : BLENDWEIGHT;\n";
            m_Has2UVs = true;
            m_HasNormals = true;
            m_HasTangents = true;
            m_HasBones = true;
            break;

        case VertexFormatType_t::Pos_2_UV_4_Bones:
            shaderCode += tab + "float2 in_uv1 : TEXCOORD0;\n" + tab + "float2 in_uv2 : TEXCOORD1;\n" + tab + "float4 in_bones : BLENDINDICES;\n" + tab + "float4 in_weights : BLENDWEIGHT;\n";
            m_Has2UVs = true;
            m_HasBones = true;
            break;
    }
}

void MaterialCodeGenerator::WriteVertexShaderOutputs(string& shaderCode)
{
    shaderCode += "    float4 position : SV_POSITION;\n";

    if (m_HasColors)
    {
        shaderCode += "    float4 color: COLOR;\n";
    }

    unsigned int semanticIndex = 0;
    if (m_HasNormals)
    {
        shaderCode += "    float3 normal : TEXCOORD" + to_string(semanticIndex++) +";\n";
    }

    if (m_Has2UVs)
    {
        shaderCode += "    float2 uv1 : TEXCOORD" + to_string(semanticIndex++) + ";\n";
        shaderCode += "    float2 uv2 : TEXCOORD" + to_string(semanticIndex++) + ";\n";
    }
    else if (m_HasUVs)
    {
        shaderCode += "    float2 uv : TEXCOORD" + to_string(semanticIndex++) + ";\n";
    }

    shaderCode += "    float3 eyePosition : TEXCOORD" + to_string(semanticIndex++) + ";\n";
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
        "cbuffer MaterialConstants_t : register(b0) {\n"
        "    float4 ambientColor;\n"
        "    float4 diffuseColor;\n"
        "    float4 specularColorAndPower;\n"
        "};\n";

    shaderCode +=
        "cbuffer PassConstants_t : register(b1) {\n"
        "    float4x4 projectionMatrix;\n"
        "    float4x4 viewMatrix;\n"
        "    float4x4 viewProjectionMatrix;\n"
        "};\n";

    string numLightsString = to_string(Sketch3D::MAX_NUM_LIGHTS);
    shaderCode +=
        "cbuffer LightConstants_t : register(b2) {\n"
        "    int numLights;\n"
        "    float4 lightPositions[" + numLightsString + "];\n"
        "    float4 lightDirections[" + numLightsString + "];\n"
        "    float4 lightColors[" + numLightsString + "];\n"
        "};\n";
}
     
void MaterialCodeGenerator::WriteFragmentShaderInputs(string& shaderCode)
{
    WriteVertexShaderOutputs(shaderCode);
}

void MaterialCodeGenerator::WriteFragmentShaderOutputs(string& shaderCode)
{
    shaderCode += "    float4 color : SV_TARGET;\n";
}

string MaterialCodeGenerator::GetTabs(int tabLevel) const
{
    string tabs = "";

    for (int i = 0; i < tabLevel * 4; i++)
    {
        tabs += " ";
    }

    return tabs;
}

}