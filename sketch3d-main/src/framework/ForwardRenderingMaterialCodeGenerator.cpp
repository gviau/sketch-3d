#include "framework/ForwardRenderingMaterialCodeGenerator.h"

namespace Sketch3D
{
void ForwardRenderingMaterialCodeGenerator::WriteVertexShaderMainBody(string& shaderCode)
{
    string tabLevel = GetTabs(1);

    string inputVariable = GetVertexShaderInputStructureVariableName();
    string outputVariable = GetVertexShaderOutputStructureVariableName();

    shaderCode += tabLevel + GetVertexShaderOutputStructureName() + " " + outputVariable + ";\n";

    shaderCode += tabLevel + outputVariable + ".position = mul(modelViewProjectionMatrix, float4(" + inputVariable + ".in_vertex, 1.0));\n";

    if (m_HasColors)
    {
        shaderCode += tabLevel + outputVariable + ".color = float4(" + inputVariable + ".in_color, 1.0);\n";
    }

    if (m_HasNormals)
    {
        shaderCode += tabLevel + outputVariable + ".normal = mul((float3x3)inverseTransposeModelViewMatrix, " + inputVariable + ".in_normal);\n";
    }

    if (m_Has2UVs)
    {
        shaderCode += tabLevel + outputVariable + ".uv1 = " + inputVariable + ".in_uv1;\n";
        shaderCode += tabLevel + outputVariable + ".uv2 = " + inputVariable + ".in_uv2;\n";
    }
    else if (m_HasUVs)
    {
        shaderCode += tabLevel + outputVariable + ".uv = " + inputVariable + ".in_uv;\n";
    }

    shaderCode += tabLevel + outputVariable + ".eyePosition = -mul(modelViewMatrix, float4(" + inputVariable + ".in_vertex, 1.0)).xyz;\n";

    shaderCode += tabLevel + "return " + outputVariable + ";\n";
}

void ForwardRenderingMaterialCodeGenerator::WriteFragmentShaderMainBody(string& shaderCode)
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

    shaderCode += tabLevel1 + GetFragmentShaderOutputStructureName() + " " + outputVariable + ";\n";

    // Ambient contribution
    shaderCode += tabLevel1 + outputVariable + ".color.xyz = ambientColor.xyz;\n";
    if (m_UsesAmbientTexture && hasUvs)
    {
        shaderCode += tabLevel1 + outputVariable + ".color.xyz *= ambientTexture.Sample(samplerState, " + uvVariable1 + ").xyz;\n";
    }

    shaderCode += "\n";

    // Diffuse contribution
    shaderCode += tabLevel1 + "float3 diffuseContribution = diffuseColor.xyz;\n";
    if (m_UsesDiffuseTexture)
    {
        shaderCode += tabLevel1 + "diffuseContribution *= diffuseTexture.Sample(samplerState, " + uvVariable1 + ").xyz;\n";
    }

    shaderCode += "\n";

    // Specular contribution
    shaderCode += tabLevel1 + "float3 specularContribution = specularColorAndPower.xyz;\n";
    if (m_UsesSpecularTexture)
    {
        shaderCode += tabLevel1 + "specularContribution *= specularTexture.Sample(samplerState, " + uvVariable1 + ").xyz;\n";
    }

    shaderCode += "\n";

    // Normal displacement (if any from the normal map)
    shaderCode += tabLevel1 + "float3 N = normalize(" + inputVariable + ".normal);\n";

    shaderCode += "\n";

    // TODO, for now, only directional and point lights are supported
    // Lighting calculations
    shaderCode +=
        tabLevel1 + "for (int i = 0; i < numLights; i++) {\n" +
            tabLevel2 + "float3 lightDirection = float3(0.0, 0.0, 0.0);\n\n" +

            tabLevel2 + "if (lightPositions[i].w > 0.5) {\n" +
                tabLevel3 + "lightDirection = normalize(mul(viewMatrix, lightPositions[i]).xyz - " + inputVariable + ".eyePosition);\n" +
            tabLevel2 + "} else {\n" +
                tabLevel3 + "lightDirection = lightDirections[i].xyz;\n" +
            tabLevel2 + "}\n\n" +

            tabLevel2 + "float nDotL = dot(lightDirection, N);\n" +
            tabLevel2 + "diffuseContribution *= nDotL * lightColors[i].xyz;\n" +
        tabLevel1 + "}\n\n";

    shaderCode += tabLevel1 + outputVariable + ".color.xyz += diffuseContribution;\n";
    // shaderCode += tabLevel1 + outputVariable + ".color.xyz += specularContribution;\n";

    shaderCode += tabLevel1 + outputVariable + ".color.w = 1.0;\n\n";
    shaderCode += tabLevel1 + "return " + outputVariable + ";\n";
}
}