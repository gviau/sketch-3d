#include "framework/MaterialCodeGenerators/ForwardRenderingMaterialCodeGenerator.h"

namespace Sketch3D
{
void ForwardRenderingMaterialCodeGenerator::WriteVertexShaderMainBody(string& shaderCode)
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

    // If we're using normal map, we transform everything in tangent space
    if (m_UsesNormalTexture && m_HasTangents)
    {
        shaderCode += tabLevel + "float3 bitangent = cross(normal, tangent);\n\n";

        shaderCode += tabLevel + outputVariable + ".eyePosition.x = dot(eyePosition, tangent);\n";
        shaderCode += tabLevel + outputVariable + ".eyePosition.y = dot(eyePosition, bitangent);\n";
        shaderCode += tabLevel + outputVariable + ".eyePosition.z = dot(eyePosition, normal);\n";

        shaderCode += "\n";
    }

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

    shaderCode += tabLevel1 + "float3 V = normalize(-" + inputVariable + ".eyePosition);\n";

    // Normal displacement (if any from the normal map)
    shaderCode += tabLevel1 + "float3 N = normalize(" + inputVariable + ".normal);\n";
    shaderCode += tabLevel1 + "float3 normal = N;\n\n";

    if (m_UsesNormalTexture && m_HasTangents)
    {
        shaderCode += tabLevel1 + "float3 T = normalize(" + inputVariable + ".tangent);\n";
        shaderCode += tabLevel1 + "float3 bitangent = cross(N, T);\n\n";

        shaderCode += tabLevel1 + "N = normalTexture.Sample(samplerState, " + uvVariable1 + ").xyz * 2.0 - 1.0;\n";
    }

    shaderCode += "\n";

    // TODO, for now, only directional and point lights are supported
    // Lighting calculations
    shaderCode += tabLevel1 + "for (int i = 0; i < numLights; i++) {\n";
    shaderCode += tabLevel2 + "float3 lightDirection = float3(0.0, 0.0, 0.0);\n\n";

    shaderCode += tabLevel2 + "if (lightPositions[i].w > 0.5) {\n";
                
    // Transform in tangent space if we're using normal maps
    if (m_UsesNormalTexture && m_HasTangents)
    {
        shaderCode += tabLevel3 + "float3 tmp = mul(viewMatrix, lightPositions[i]);\n";
        shaderCode += tabLevel3 + "float3 lightPosition = float3(0.0, 0.0, 0.0);\n";
        shaderCode += tabLevel3 + "lightPosition.x = dot(tmp, T);\n";
        shaderCode += tabLevel3 + "lightPosition.y = dot(tmp, bitangent);\n";
        shaderCode += tabLevel3 + "lightPosition.z = dot(tmp, normal);\n\n";
        
        shaderCode += tabLevel3 + "lightDirection = normalize(lightPosition - " + inputVariable + ".eyePosition);\n";
    }
    else
    {
        shaderCode += tabLevel3 + "lightDirection = normalize(mul(viewMatrix, lightPositions[i]).xyz - " + inputVariable + ".eyePosition);\n";
    }
                
    shaderCode += tabLevel2 + "} else {\n";

    // Transform in tangent space if we're using normal maps
    if (m_UsesNormalTexture && m_HasTangents)
    {
        shaderCode += tabLevel3 + "float3 lightDirection = float3(0.0, 0.0, 0.0);\n";
        shaderCode += tabLevel3 + "lightDirection.x = dot(lightDirections[i].xyz, T);\n";
        shaderCode += tabLevel3 + "lightDirection.y = dot(lightDirections[i].xyz, bitangent);\n";
        shaderCode += tabLevel3 + "lightDirection.z = dot(lightDirections[i].xyz, normal);\n";
    }
    else
    {
        shaderCode += tabLevel3 + "lightDirection = lightDirections[i].xyz;\n";
    }

    shaderCode += tabLevel2 + "}\n\n";

    shaderCode += tabLevel2 + "float nDotL = dot(lightDirection, N);\n";
    shaderCode += tabLevel2 + "diffuseContribution *= nDotL * lightColors[i].xyz;\n";

    shaderCode += tabLevel2 + "float3 H = normalize(lightDirection + V);\n";
    shaderCode += tabLevel2 + "float nDotH = dot(N, H);\n\n";
            
    shaderCode += tabLevel2 + "specularContribution *= pow(max(nDotH, 0.0), specularColorAndPower.w) * lightColors[i].xyz;\n";

    shaderCode += tabLevel1 + "}\n\n";

    shaderCode += tabLevel1 + outputVariable + ".color.xyz += diffuseContribution;\n";
    shaderCode += tabLevel1 + outputVariable + ".color.xyz += specularContribution * min(1.0, specularColorAndPower.w);\n";

    shaderCode += tabLevel1 + outputVariable + ".color.w = 1.0;\n\n";
    shaderCode += tabLevel1 + "return " + outputVariable + ";\n";
}
}