#include "framework/ForwardRenderingPipeline.h"

#include "framework/Camera.h"
#include "framework/ForwardRenderingMaterialCodeGenerator.h"
#include "framework/Light.h"
#include "framework/Material.h"
#include "framework/Mesh.h"
#include "framework/Scene.h"

#include "render/Buffer.h"
#include "render/ConstantBuffers.h"
#include "render/HardwareResourceCreator.h"
#include "render/RenderContext.h"
#include "render/RenderDevice.h"

#include "system/Logger.h"

namespace Sketch3D
{
ForwardRenderingPipeline::ForwardRenderingPipeline(const shared_ptr<RenderContext>& renderContext, const shared_ptr<RenderDevice>& renderDevice)
    : RenderingPipeline(renderContext, renderDevice)
{
}

ForwardRenderingPipeline::~ForwardRenderingPipeline()
{
    delete m_MaterialCodeGenerator;
}

bool ForwardRenderingPipeline::Initialize()
{
    RenderDevice* renderDevice = m_RenderDevice.get();
    if (renderDevice == nullptr)
    {
        Logger::GetInstance()->Error("Couldn't initialize the ForwardRenderingPipeline object because the render device is null");
        return false;
    }

    m_MaterialCodeGenerator = new ForwardRenderingMaterialCodeGenerator;
    m_VertexShaderConstantBuffersSlots = m_MaterialCodeGenerator->GetVertexShaderConstantBuffersSlots();
    m_FragmentShaderConstantBuffersSlots = m_MaterialCodeGenerator->GetFragmentShaderConstantBuffersSlots();

    m_PassConstants = renderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();
    m_DrawConstants = renderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();
    m_LightConstants = renderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();
    
    m_PassConstants->Initialize(nullptr, true, false, sizeof(PassConstants_t));
    m_DrawConstants->Initialize(nullptr, true, false, sizeof(DrawConstants_t));
    m_LightConstants->Initialize(nullptr, true, false, sizeof(LightConstants_t));

    return true;
}

void ForwardRenderingPipeline::RenderSceneFromCamera(Camera& camera, const Scene& scene)
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

    m_RenderDevice->ClearRenderTargets(Vector4(0.1f, 0.1f, 0.1f));
    m_RenderDevice->ClearDepthStencil(true, false, 1.0f, 0);

    // Draw the visual nodes
    const vector<shared_ptr<VisualNode>>& visualNodesToDraw = scene.GetRootNode().GetChildren();

    for (const shared_ptr<VisualNode>& visualNodeToDraw : visualNodesToDraw)
    {
        VisualNode* visualNode = visualNodeToDraw.get();
        if (visualNode == nullptr)
        {
            continue;
        }

        Mesh* meshToDraw = visualNode->GetMesh().get();
        if (meshToDraw == nullptr)
        {
            continue;
        }

        Matrix4x4 modelMatrix = visualNode->GetModelMatrix();

        // Fill the draw constants
        DrawConstants_t* drawConstants = (DrawConstants_t*)m_DrawConstants->Map(MapFlag_t::WRITE_DISCARD);

        Matrix4x4 modelViewMatrix = m_CurrentRenderingContext.m_ViewMatrix * modelMatrix;
        drawConstants->modelMatrix = modelMatrix;
        drawConstants->modelViewMatrix = modelViewMatrix;

        const Vector3& scale = visualNode->GetScale();
        bool isScalingUniform = !(fabsf(scale.x - scale.y) > EPSILON || fabsf(scale.x - scale.z) > EPSILON || fabs(scale.y - scale.z) > EPSILON);

        if (isScalingUniform)
        {
            drawConstants->transposedInverseModelViewMatrix = modelViewMatrix;
        }
        else
        {
            drawConstants->transposedInverseModelViewMatrix = modelViewMatrix.Inverse().Transpose();
        }

        drawConstants->modelViewProjectionMatrix = m_CurrentRenderingContext.m_ProjectionMatrix * modelViewMatrix;

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

        meshToDraw->SetConstantBuffersForAllMaterials(vertexShaderConstantBuffers, fragmentShaderConstantBuffers,
                                                      m_FragmentShaderConstantBuffersSlots[typeid(MaterialConstants_t).name()]);

        meshToDraw->SetAmbientColorForAllSubMeshes(visualNode->GetAmbientColor());
        meshToDraw->SetDiffuseColorForAllSubMeshes(visualNode->GetDiffuseColor());
        meshToDraw->SetSpecularColorForAllSubMeshes(visualNode->GetSpecularColor());
        meshToDraw->SetSpecularPowerForAllSubMeshes(visualNode->GetSpecularPower());

        // Draw the mesh
        meshToDraw->Draw(m_RenderDevice);
    }

    m_RenderContext->SwapBuffers();
}
}