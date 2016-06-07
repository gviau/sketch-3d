#include "framework/RenderingPipelines/DeferredRenderingPipeline.h"

#include "framework/Camera.h"
#include "framework/Light.h"
#include "framework/Material.h"
#include "framework/Mesh.h"
#include "framework/Scene.h"
#include "framework/SimpleObject.h"
#include "framework/SkinnedMesh.h"
#include "framework/SubMesh.h"

#include "framework/MaterialCodeGenerators/DeferredRenderingMaterialCodeGenerator.h"

#include "render/Buffer.h"
#include "render/ConstantBuffers.h"
#include "render/HardwareResourceCreator.h"
#include "render/RenderContext.h"
#include "render/RenderDevice.h"
#include "render/RenderTarget.h"
#include "render/SamplerState.h"
#include "render/Shader.h"

#include "system/Logger.h"

#include <algorithm>
using namespace std;

namespace Sketch3D
{
DeferredRenderingPipeline::DeferredRenderingPipeline(const shared_ptr<RenderContext>& renderContext, const shared_ptr<RenderDevice>& renderDevice)
    : RenderingPipeline(renderContext, renderDevice)
{
}

DeferredRenderingPipeline::~DeferredRenderingPipeline()
{
}

bool DeferredRenderingPipeline::Initialize()
{
    RenderDevice* renderDevice = m_RenderDevice.get();
    if (renderDevice == nullptr)
    {
        Logger::GetInstance()->Error("Couldn't initialize the DeferredRenderingPipeline object because the render device is null");
        return false;
    }

    m_MaterialCodeGenerator = new DeferredRenderingMaterialCodeGenerator;
    m_VertexShaderConstantBuffersSlots = m_MaterialCodeGenerator->GetVertexShaderConstantBuffersSlots();
    m_FragmentShaderConstantBuffersSlots = m_MaterialCodeGenerator->GetFragmentShaderConstantBuffersSlots();

    m_PassConstants = renderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();
    m_DrawConstants = renderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();
	m_BoneConstants = renderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();
    m_LightConstants = renderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();
    
    m_PositionsRenderTarget = renderDevice->GetHardwareResourceCreator()->CreateRenderTarget();
    m_NormalsRenderTarget = renderDevice->GetHardwareResourceCreator()->CreateRenderTarget();
    m_AlbedosRenderTarget = renderDevice->GetHardwareResourceCreator()->CreateRenderTarget();

    m_PassConstants->Initialize(nullptr, true, false, sizeof(PassConstants_t));
    m_DrawConstants->Initialize(nullptr, true, false, sizeof(DrawConstants_t));
	m_BoneConstants->Initialize(nullptr, true, false, sizeof(BoneConstants_t));
    m_LightConstants->Initialize(nullptr, true, false, sizeof(LightConstants_t));

    // GBuffer creation
    const RenderParameters_t& renderParameters = m_RenderContext->GetRenderParameters();
    m_PositionsRenderTarget->Initialize(renderParameters.width, renderParameters.height, TextureFormat_t::RGBA32F);
    m_NormalsRenderTarget->Initialize(renderParameters.width, renderParameters.height, TextureFormat_t::RGBA32F);
    m_AlbedosRenderTarget->Initialize(renderParameters.width, renderParameters.height, TextureFormat_t::RGBA32F);

    m_GBuffer.push_back(m_PositionsRenderTarget);
    m_GBuffer.push_back(m_NormalsRenderTarget);
    m_GBuffer.push_back(m_AlbedosRenderTarget);

    CreateVertexShader();
    CreateFragmentShader();

    m_GBufferSamplerState = renderDevice->GetHardwareResourceCreator()->CreateSamplerState();
    m_GBufferSamplerState->Initialize(FilterMode_t::NEAREST, AddressMode_t::BORDER, AddressMode_t::BORDER, AddressMode_t::BORDER,
                                      ComparisonFunction_t::EQUAL);

    InitializeSimpleObjects(m_RenderDevice, m_MaterialCodeGenerator);

    return true;
}

void DeferredRenderingPipeline::RenderSceneFromCamera(Camera& camera, const Scene& scene, double deltaTime)
{
    ConstructRenderingPipelineContext(camera, scene, m_CurrentRenderingContext);

    // Prepare the pass constants
    PassConstants_t* passConstants = (PassConstants_t*)m_PassConstants->Map(MapFlag_t::WRITE_DISCARD);

    passConstants->projectionMatrix = m_CurrentRenderingContext.m_ProjectionMatrix;
    passConstants->viewMatrix = m_CurrentRenderingContext.m_ViewMatrix;
    passConstants->viewProjectionMatrix = m_CurrentRenderingContext.m_ProjectionMatrix * m_CurrentRenderingContext.m_ViewMatrix;

    m_PassConstants->Unmap();

    // Prepare the light constants
    const vector<shared_ptr<Light>>& lights = scene.GetLights();
    size_t numLights = min<size_t>(lights.size(), 8);

    LightConstants_t* lightConstants = (LightConstants_t*)m_LightConstants->Map(MapFlag_t::WRITE_DISCARD);

    lightConstants->numLights = numLights;

    for (size_t i = 0; i < numLights; i++)
    {
        Vector4 position, direction, color;
        lights[i]->GetLightConstants(position, direction, color);

        lightConstants->lightPositions[i] = position;
        lightConstants->lightDirections[i] = direction;
        lightConstants->lightColors[i] = color;
    }

    m_LightConstants->Unmap();

    // Draw everything in the gbuffer
    m_RenderDevice->SetRenderTargets(m_GBuffer);
    m_RenderDevice->ClearRenderTargets(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    m_RenderDevice->ClearDepthStencil(true, false, 1.0f, 0);

    const vector<shared_ptr<VisualNode>>& visualNodesToDraw = scene.GetRootNode().GetChildren();

    for (const shared_ptr<VisualNode>& visualNodeToDraw : visualNodesToDraw)
    {
        VisualNode* visualNode = visualNodeToDraw.get();
        if (visualNode == nullptr)
        {
            continue;
        }

		if (!visualNode->IsNodeValidForRendering())
		{
			continue;
		}

		visualNode->FillRenderingPipelineContext(m_CurrentRenderingContext, deltaTime);

		// Fill the draw constants
		DrawConstants_t* drawConstants = (DrawConstants_t*)m_DrawConstants->Map(MapFlag_t::WRITE_DISCARD);

		drawConstants->modelMatrix = m_CurrentRenderingContext.m_ModelMatrix;
		drawConstants->modelViewMatrix = m_CurrentRenderingContext.m_ModelViewMatrix;
		drawConstants->transposedInverseModelViewMatrix = m_CurrentRenderingContext.m_TransposedInverseModelViewMatrix;
		drawConstants->modelViewProjectionMatrix = m_CurrentRenderingContext.m_ModelViewProjectionMatrix;

		m_DrawConstants->Unmap();

		// Set the material's constant buffers
		vector<shared_ptr<ConstantBuffer>> vertexShaderConstantBuffers;
		vector<shared_ptr<ConstantBuffer>> fragmentShaderConstantBuffers;
		vertexShaderConstantBuffers.resize(m_VertexShaderConstantBuffersSlots.size());
		fragmentShaderConstantBuffers.resize(m_FragmentShaderConstantBuffersSlots.size());

		vertexShaderConstantBuffers[m_VertexShaderConstantBuffersSlots[typeid(PassConstants_t).name()]] = m_PassConstants;
		vertexShaderConstantBuffers[m_VertexShaderConstantBuffersSlots[typeid(DrawConstants_t).name()]] = m_DrawConstants;

		fragmentShaderConstantBuffers[m_FragmentShaderConstantBuffersSlots[typeid(PassConstants_t).name()]] = m_PassConstants;
		fragmentShaderConstantBuffers[m_FragmentShaderConstantBuffersSlots[typeid(LightConstants_t).name()]] = m_LightConstants;

		// If it's a skinned mesh, get its transformation matrices
		if (m_CurrentRenderingContext.m_Flags.m_IsSkinnedMesh)
		{
			BoneConstants_t* boneConstants = (BoneConstants_t*)m_BoneConstants->Map(MapFlag_t::WRITE_DISCARD);

			for (size_t i = 0; i < m_CurrentRenderingContext.m_BoneTransformationMatrices.size(); i++)
			{
				boneConstants->boneTransformationMatrices[i] = m_CurrentRenderingContext.m_BoneTransformationMatrices[i];
			}

			m_BoneConstants->Unmap();

			vertexShaderConstantBuffers[m_VertexShaderConstantBuffersSlots[typeid(BoneConstants_t).name()]] = m_BoneConstants;
		}

		visualNode->RenderNode(m_RenderDevice,
							   vertexShaderConstantBuffers,
							   fragmentShaderConstantBuffers,
							   m_FragmentShaderConstantBuffersSlots[typeid(MaterialConstants_t).name()]);
    }

    // Lighting pass
    m_RenderDevice->SetDefaultRenderTargetAndDepthStencilBuffer();
    m_RenderDevice->ClearRenderTargets(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    m_RenderDevice->ClearDepthStencil(true, false, 1.0f, 0);

    const shared_ptr<VertexBuffer>& vertexBuffer = g_FullscreenQuad->GetSubMesh(0)->GetVertexBuffer();
    const shared_ptr<IndexBuffer>& indexBuffer = g_FullscreenQuad->GetSubMesh(0)->GetIndexBuffer();

    m_RenderDevice->SetVertexShader(m_FullscreenQuadVertexShader);

    m_RenderDevice->SetFragmentShader(m_LightingAccumulationFragmentShader);
    m_RenderDevice->SetFragmentShaderSamplerState(m_GBufferSamplerState, 0);
    m_RenderDevice->SetFragmentShaderTexture(m_PositionsRenderTarget, 0);
    m_RenderDevice->SetFragmentShaderTexture(m_NormalsRenderTarget, 1);
    m_RenderDevice->SetFragmentShaderTexture(m_AlbedosRenderTarget, 2);
    m_RenderDevice->SetFragmentShaderConstantBuffer(m_PassConstants, 0);
    m_RenderDevice->SetFragmentShaderConstantBuffer(m_LightConstants, 1);

    m_RenderDevice->DrawIndexed(PrimitiveTopology_t::TRIANGLELIST, vertexBuffer, indexBuffer, 0, 0);

    m_RenderContext->SwapBuffers();
}

void DeferredRenderingPipeline::CreateVertexShader()
{
    string shaderCode =
        "struct VS_INPUT {\n"
            "\tfloat3 in_vertex : POSITION;\n"
        "};\n\n"

        "struct VS_OUTPUT {\n"
            "\tfloat4 position : SV_POSITION;\n"
            "\tfloat2 uv : TEXCOORD;\n"
        "};\n\n"

        "VS_OUTPUT main(VS_INPUT input) {\n"
            "\tVS_OUTPUT output;\n"
            "\toutput.position = float4(input.in_vertex.xy, 0.0, 1.0);\n"
            "\toutput.uv = input.in_vertex.xy * 0.5 + 0.5;\n"
            "\treturn output;\n"
        "}";

    m_FullscreenQuadVertexShader = m_RenderDevice->GetHardwareResourceCreator()->CreateVertexShader();
    m_FullscreenQuadVertexShader->InitializeFromSource(shaderCode);
}

void DeferredRenderingPipeline::CreateFragmentShader()
{
    string numLightsString = to_string(Sketch3D::MAX_NUM_LIGHTS);

    string shaderCode =
        "SamplerState samplerState : register(s0);\n\n"
        
        "Texture2D positionsBuffer : register(t0);\n"
        "Texture2D normalsBuffer : register(t1);\n"
        "Texture2D albedosBuffer : register(t2);\n\n"

        "cbuffer PassConstants_t : register(b0) {\n"
            "\tfloat4x4 projectionMatrix;\n"
            "\tfloat4x4 viewMatrix;\n"
            "\tfloat4x4 viewProjectionMatrix;\n"
        "};\n\n"

        "cbuffer LightConstants_t : register(b1) {\n"
            "\tint numLights;\n"
            "\tfloat4 lightPositions[" + numLightsString + "];\n"
            "\tfloat4 lightDirections[" + numLightsString + "];\n"
            "\tfloat4 lightColors[" + numLightsString + "];\n"
        "};\n\n"

        "struct PS_INPUT {\n"
            "\tfloat4 position : SV_POSITION;\n"
            "\tfloat2 uv : TEXCOORD;\n"
        "};\n"

        "struct PS_OUTPUT {\n"
            "\tfloat4 color : SV_TARGET;\n"
        "};\n\n"

        "PS_OUTPUT main(PS_INPUT input) {\n"
            "\tPS_OUTPUT output;\n\n"

            "\tfloat2 uv = input.uv;\n"
            "\tuv.y = 1.0 - uv.y;\n\n"

            "\tfloat3 eyePosition = positionsBuffer.Sample(samplerState, uv).xyz;\n"
            "\tfloat3 normal = normalsBuffer.Sample(samplerState, uv).xyz;\n"
            "\tfloat3 albedo = albedosBuffer.Sample(samplerState, uv).xyz;\n\n"

            "\tif (numLights == 0) {\n"
            "\t\toutput.color = float4(albedo, 1.0);\n"
            "\t} else {\n"
            "\t\toutput.color = float4(0.0, 0.0, 0.0, 1.0);\n"
            "\t}\n\n";

    // Lighting accumulation
    shaderCode +=
        "\tfor (int i = 0; i < numLights; i++) {\n"
            "\t\tfloat3 lightDirection = float3(0.0, 0.0, 0.0);\n\n"

            "\t\tif (lightPositions[i].w > 0.5) {\n"
                "\t\t\tlightDirection = normalize(mul(viewMatrix, lightPositions[i]).xyz - eyePosition);\n"
                
            "\t\t} else {\n"
                "\t\t\tlightDirection = lightDirections[i].xyz;\n"

            "\t\t}\n\n"

            "\t\tfloat nDotL = max(dot(normal, lightDirection), 0.0);\n"

            "\t\toutput.color.xyz += albedo * nDotL * lightColors[i].xyz;\n"

        "\t}\n";

    shaderCode +=
            "\treturn output;\n"
        "}";

    m_LightingAccumulationFragmentShader = m_RenderDevice->GetHardwareResourceCreator()->CreateFragmentShader();
    m_LightingAccumulationFragmentShader->InitializeFromSource(shaderCode);
}

}