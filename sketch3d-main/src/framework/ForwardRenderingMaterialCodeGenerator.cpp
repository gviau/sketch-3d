#include "framework/ForwardRenderingMaterialCodeGenerator.h"

namespace Sketch3D
{
void ForwardRenderingMaterialCodeGenerator::WriteVertexShaderMainBody(string& shaderCode)
{
    string inputVariable = GetVertexShaderInputStructureVariableName();
    string outputVariable = GetVertexShaderOutputStructureVariableName();

    shaderCode += GetVertexShaderOutputStructureName() + " " + outputVariable + ";\n";

    shaderCode += outputVariable + ".position = mul(modelViewProjectionMatrix, float4(" + inputVariable + ".in_vertex, 1.0));\n";

    if (m_HasColors)
    {
        shaderCode += outputVariable + ".color = float4(" + inputVariable + ".in_color, 1.0);\n";
    }

    if (m_HasNormals)
    {
        shaderCode += outputVariable + ".normal = mul((float3x3)inverseTransposeModelViewMatrix, " + inputVariable + ".in_normal);\n";
    }

    if (m_Has2UVs)
    {
        shaderCode += outputVariable + ".uv1 = " + inputVariable + ".in_uv1;\n";
        shaderCode += outputVariable + ".uv2 = " + inputVariable + ".in_uv2;\n";
    }
    else if (m_HasUVs)
    {
        shaderCode += outputVariable + ".uv = " + inputVariable + ".in_uv;\n";
    }

    shaderCode += outputVariable + ".eyePosition = -mul(modelViewMatrix, float4(" + inputVariable + ".in_vertex, 1.0)).xyz;\n";

    shaderCode += "return " + outputVariable;
}

void ForwardRenderingMaterialCodeGenerator::WriteFragmentShaderMainBody(string& shaderCode)
{
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

    shaderCode += GetFragmentShaderOutputStructureName() + " " + outputVariable + ";\n";

    // Ambient contribution
    shaderCode += outputVariable + ".color.xyz = ambientColor.xyz;\n";
    if (m_UsesAmbientTexture && hasUvs)
    {
        shaderCode += outputVariable + ".color.xyz *= ambientTexture.Sample(samplerState, " + uvVariable1 + ").xyz;\n";
    }

    // Diffuse contribution
    shaderCode += "float3 diffuseContribution = diffuseColor.xyz;\n";
    if (m_UsesDiffuseTexture)
    {
        shaderCode += "diffuseContribution *= diffuseTexture.Sample(samplerState, " + uvVariable1 + ").xyz;\n";
    }

    // Specular contribution
    shaderCode += "float3 specularContribution = specularColorAndPower.xyz;\n";
    if (m_UsesSpecularTexture)
    {
        shaderCode += "specularContribution *= specularTexture.Sample(samplerState, " + uvVariable1 + ").xyz;\n";
    }

    // Normal displacement (if any from the normal map)

    // TODO, for now, only directional and point lights are supported
    // Lighting calculations
    shaderCode +=
        "for (int i = 0; i < numLights; i++) {\n"
            "float3 lightDirection = float3(0.0, 0.0, 0.0);\n"

            "if (lightPositions[i].w > 0.5) {\n"
                "lightDirection = normalize(mul(viewMatrix, lightPositions[i]) - " + inputVariable + ".eyePosition);\n"
            "} else {\n"
                "lightDirection = lightDirections[i].xyz;\n"
            "}\n"

            "float nDotL = dot(lightDirection, " + inputVariable + ".normal);\n"
            "diffuseContribution *= nDotL * lightColors[i].xyz;\n"
        "}\n";

    shaderCode += inputVariable + ".color.xyz += diffuseContribution;\n";

    shaderCode += inputVariable + ".color.w = 1.0;\n";
    shaderCode += "return " + outputVariable;
}
}