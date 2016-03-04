#include "framework/MaterialCodeGenerators/DeferredRenderingMaterialCodeGenerator.h"

namespace Sketch3D
{
void DeferredRenderingMaterialCodeGenerator::WriteVertexShaderMainBody(string& shaderCode)
{
    string tabLevel = GetTabs(1);

    string inputVariable = GetVertexShaderInputStructureVariableName();
    string outputVariable = GetVertexShaderOutputStructureVariableName();

    shaderCode += tabLevel + GetVertexShaderOutputStructureName() + " " + outputVariable + ";\n\n";

    shaderCode += tabLevel + outputVariable + ".position = mul(modelViewProjectionMatrix, float4(" + inputVariable + ".in_vertex, 1.0));\n\n";

    if (m_HasColors)
    {
        shaderCode += tabLevel + outputVariable + ".color = float4(" + inputVariable + ".in_color, 1.0);\n\n";
    }

    if (m_HasNormals)
    {
        shaderCode += tabLevel + "float3 normal = normalize(mul((float3x3)transposedInverseModelViewMatrix, " + inputVariable + ".in_normal));\n";
        shaderCode += tabLevel + outputVariable + ".normal = normal;\n\n";
    }

    if (m_HasTangents)
    {
        shaderCode += tabLevel + "float3 tangent = normalize(mul((float3x3)modelViewMatrix, " + inputVariable + ".in_tangent));\n";
        shaderCode += tabLevel + outputVariable + ".tangent = tangent;\n\n";
    }

    if (m_Has2UVs)
    {
        shaderCode += tabLevel + outputVariable + ".uv1 = " + inputVariable + ".in_uv1;\n";
        shaderCode += tabLevel + outputVariable + ".uv2 = " + inputVariable + ".in_uv2;\n\n";
    }
    else if (m_HasUVs)
    {
        shaderCode += tabLevel + outputVariable + ".uv = " + inputVariable + ".in_uv;\n\n";
    }

    shaderCode += tabLevel + "float3 eyePosition = mul(modelViewMatrix, float4(" + inputVariable + ".in_vertex, 1.0)).xyz;\n";
    shaderCode += tabLevel + outputVariable + ".eyePosition = eyePosition;\n\n";

    shaderCode += tabLevel + "return " + outputVariable + ";\n";
}

void DeferredRenderingMaterialCodeGenerator::WriteFragmentShaderMainBody(string& shaderCode)
{
    string tabLevel1 = GetTabs(1);
    string tabLevel2 = GetTabs(2);
    string tabLevel3 = GetTabs(3);

    string inputVariable = GetFragmentShaderInputStructureVariableName();
    string outputVariable = GetFragmentShaderOutputStructureVariableName();
    
    string uvVariable1 = "";
    string uvVariable2 = "";
    if (m_Has2UVs)
    {
        uvVariable1 = inputVariable + ".uv1";
        uvVariable2 = inputVariable + ".uv2";
    }
    else if (m_HasUVs)
    {
        uvVariable1 = inputVariable + ".uv";
    }

    bool hasUvs = (m_HasUVs || m_Has2UVs);

    shaderCode += tabLevel1 + GetFragmentShaderOutputStructureName() + " " + outputVariable + ";\n\n";

    shaderCode += tabLevel1 + outputVariable + ".positions = float4(" + inputVariable + ".eyePosition, 1.0);\n\n";

    // Diffuse contribution
    shaderCode += tabLevel1 + "float3 diffuseContribution = diffuseColor.xyz;\n";
    if (m_UsesDiffuseTexture)
    {
        shaderCode += tabLevel1 + "diffuseContribution *= diffuseTexture.Sample(samplerState, " + uvVariable1 + ").xyz;\n";
    }
    
    shaderCode += tabLevel1 + outputVariable + ".albedos = float4(diffuseContribution, 1.0);\n";

    shaderCode += "\n";

    // Normal displacement (if any from the normal map)
    shaderCode += tabLevel1 + "float3 N = normalize(" + inputVariable + ".normal);\n";
    shaderCode += tabLevel1 + "float3 normal = N;\n";

    if (m_UsesNormalTexture && m_HasTangents)
    {
        shaderCode += "\n";

        shaderCode += tabLevel1 + "float3 T = normalize(" + inputVariable + ".tangent);\n";
        shaderCode += tabLevel1 + "float3 bitangent = cross(N, T);\n\n";

        shaderCode += tabLevel1 + "N = normalTexture.Sample(samplerState, " + uvVariable1 + ").xyz * 2.0 - 1.0;\n\n";

        shaderCode += tabLevel1 + "float3x3 tbn = float3x3(float3(T.x, bitangent.x, normal.x), float3(T.y, bitangent.y, normal.y), float3(T.z, bitangent.z, normal.z));\n\n";

        shaderCode += tabLevel1 + "normal = normalize(mul(tbn, N));\n";
    }

    shaderCode += "\n";

    shaderCode += outputVariable + ".normals = float4(normal, 0.0);\n";

    shaderCode += tabLevel1 + "return " + outputVariable + ";\n";
}

void DeferredRenderingMaterialCodeGenerator::WriteFragmentShaderConstantBuffers(string& shaderCode)
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

    shaderCode += "\n";

    shaderCode +=
        "cbuffer MaterialConstants_t : register(b0) {\n"
        "    float4 ambientColor;\n"
        "    float4 diffuseColor;\n"
        "    float4 specularColorAndPower;\n"
        "};\n\n";

    shaderCode +=
        "cbuffer PassConstants_t : register(b1) {\n"
        "    float4x4 projectionMatrix;\n"
        "    float4x4 viewMatrix;\n"
        "    float4x4 viewProjectionMatrix;\n"
        "};\n\n";
}

void DeferredRenderingMaterialCodeGenerator::WriteFragmentShaderOutputs(string& shaderCode)
{
    shaderCode += "    float4 positions : SV_TARGET0;\n";
    shaderCode += "    float4 normals : SV_TARGET1;\n";
    shaderCode += "    float4 albedos : SV_TARGET2;\n";
}

}