#include <math/Quaternion.h>
#include <math/Vector3.h>
#include <math/Vector4.h>

#include <framework/Camera.h>
#include <framework/Mesh.h>
#include <framework/SubMesh.h>

#include <render/Buffer.h>
#include <render/ConstantBuffers.h>
#include <render/HardwareResourceCreator.h>
#include <render/RenderContext.h>
#include <render/RenderDevice.h>
#include <render/RenderTarget.h>
#include <render/Shader.h>

#include <system/Logger.h>
#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <string>
#include <vector>
using namespace std;

#ifdef OIS_AVAILABLE
#include <OIS.h>
#endif

struct ALIGNED_(16) ScreenParameters_t
{
    Vector4 ScreenParams;
};

#if PLATFORM == PLATFORM_WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif

    Window window("Sample_CelShading", 1024, 768, true);
    RenderParameters_t renderParameters;
    renderParameters.width = 1024;
    renderParameters.height = 768;
    renderParameters.displayFormat = DisplayFormat_t::A8R8G8B8;
    renderParameters.refreshRate = 0;
    renderParameters.depthStencilBits = DepthStencilBits_t::D32;

    shared_ptr<RenderContext> renderContext = CreateRenderContext(RenderSystem_t::DIRECT3D11);
    renderContext->Initialize(window, renderParameters);

    shared_ptr<RenderDevice> renderDevice;
    if (!CreateRenderDevice(renderContext, renderDevice))
    {
        Logger::GetInstance()->Error("Couldn't create render device");
        return 1;
    }
    renderDevice->Initialize(renderContext);

    // Load the mesh
    shared_ptr<Mesh> teapotMesh;
    LoadMeshFromFile("Media/teapot.nff", renderDevice, teapotMesh);

    // Create the material
    shared_ptr<VertexShader> vertexShader = renderDevice->GetHardwareResourceCreator()->CreateVertexShader();
    shared_ptr<FragmentShader> fragmentShader = renderDevice->GetHardwareResourceCreator()->CreateFragmentShader();

    vertexShader->InitializeFromFile("Shaders/CelShading/vert.hlsl");
    fragmentShader->InitializeFromFile("Shaders/CelShading/frag.hlsl");

    // Create a fullscreen quad mesh
    Vertex_Pos_t vertices[4];
    vertices[0].position.x = -1.0f; vertices[0].position.y = -1.0f; vertices[0].position.z = 0.0f;
    vertices[1].position.x = -1.0f; vertices[1].position.y =  1.0f; vertices[1].position.z = 0.0f;
    vertices[2].position.x =  1.0f; vertices[2].position.y =  1.0f; vertices[2].position.z = 0.0f;
    vertices[3].position.x =  1.0f; vertices[3].position.y = -1.0f; vertices[3].position.z = 0.0f;

    shared_ptr<VertexBuffer> fullScreenQuadVertices = renderDevice->GetHardwareResourceCreator()->CreateVertexBuffer();
    fullScreenQuadVertices->Initialize(vertices, false, false, VertexFormatType_t::Pos, 4);

    unsigned short indices[6];
    indices[0] = 0; indices[1] = 2; indices[2] = 1;
    indices[3] = 0; indices[4] = 3; indices[5] = 2;

    shared_ptr<IndexBuffer> fullScreenQuadIndices = renderDevice->GetHardwareResourceCreator()->CreateIndexBuffer();
    fullScreenQuadIndices->Initialize(indices, false, false, IndexFormat_t::INT_2_BYTES, 6);

    Camera camera;
    camera.LookAtRightHanded(Vector3(0.0f, 3.0f, 7.5f), Vector3::LOOK);

    // Initialize OIS if available
#ifdef OIS_AVAILABLE
    size_t windowHandle = (size_t)window.GetHandle();

    OIS::ParamList paramList;
    paramList.insert(pair<string, string>("WINDOW", to_string(windowHandle)));

    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(paramList);

    OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, false));
#endif

    float teapotAngle = 0.0f;
    const vector<shared_ptr<SubMesh>>& subMeshes = teapotMesh->GetSubMeshes();

    PassConstants_t initialPassConstants;
    initialPassConstants.projectionMatrix = renderDevice->CalculatePerspectiveProjectionFOVRightHanded(60.0f, (float)renderParameters.width / (float)renderParameters.height, 0.01f, 1000.0f);
    initialPassConstants.viewMatrix = camera.GetViewMatrix();

    shared_ptr<ConstantBuffer> passConstants = renderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();
    passConstants->Initialize(&initialPassConstants, false, false, sizeof(PassConstants_t));

    shared_ptr<ConstantBuffer> drawConstants = renderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();
    drawConstants->Initialize(nullptr, true, false, sizeof(DrawConstants_t));

    // Create the shaders for the outline effect
    shared_ptr<VertexShader> fullScreenQuadShader = renderDevice->GetHardwareResourceCreator()->CreateVertexShader();
    shared_ptr<FragmentShader> simplePassShader = renderDevice->GetHardwareResourceCreator()->CreateFragmentShader();
    shared_ptr<FragmentShader> sobelFilterShader = renderDevice->GetHardwareResourceCreator()->CreateFragmentShader();
    shared_ptr<FragmentShader> imageMultiplicationShader = renderDevice->GetHardwareResourceCreator()->CreateFragmentShader();

    fullScreenQuadShader->InitializeFromFile("Shaders/CelShading/fullScreenQuad.hlsl");
    simplePassShader->InitializeFromFile("Shaders/CelShading/simplePass.hlsl");
    sobelFilterShader->InitializeFromFile("Shaders/CelShading/sobelFilter.hlsl");
    imageMultiplicationShader->InitializeFromFile("Shaders/CelShading/imageMultiplication.hlsl");

    // Render target creation
    shared_ptr<RenderTarget> renderTarget = renderDevice->GetHardwareResourceCreator()->CreateRenderTarget();
    renderTarget->Initialize(renderParameters.width, renderParameters.height, TextureFormat_t::RGBA32);

    vector<shared_ptr<RenderTarget>> firstPassRenderTargets;
    firstPassRenderTargets.push_back(renderTarget);

    // Simple pass render target
    shared_ptr<RenderTarget> simplePassRenderTarget = renderDevice->GetHardwareResourceCreator()->CreateRenderTarget();
    simplePassRenderTarget->Initialize(renderParameters.width, renderParameters.height, TextureFormat_t::RGBA32);

    vector<shared_ptr<RenderTarget>> simplePassRenderTargets;
    simplePassRenderTargets.push_back(simplePassRenderTarget);

    // Sobel filter render target
    shared_ptr<RenderTarget> sobelFilterResult = renderDevice->GetHardwareResourceCreator()->CreateRenderTarget();
    sobelFilterResult->Initialize(renderParameters.width, renderParameters.height, TextureFormat_t::RGBA32);

    vector<shared_ptr<RenderTarget>> sobelFilterRenderTargets;
    sobelFilterRenderTargets.push_back(sobelFilterResult);

    shared_ptr<SamplerState> samplerState = renderDevice->GetHardwareResourceCreator()->CreateSamplerState();
    samplerState->Initialize(FilterMode_t::NEAREST, AddressMode_t::BORDER, AddressMode_t::BORDER, AddressMode_t::BORDER, ComparisonFunction_t::EQUAL);


    // Constant buffer for the screen parameters
    ScreenParameters_t initialScreenParameters;
    initialScreenParameters.ScreenParams = Vector4(renderParameters.width * 0.5f, renderParameters.height * 0.5f, 0.0f, 0.0f);

    shared_ptr<ConstantBuffer> screenParameters = renderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();
    screenParameters->Initialize(&initialScreenParameters, false, false, sizeof(initialScreenParameters));

    while (window.IsOpen()) {
        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

#ifdef OIS_AVAILABLE
        keyboard->capture();

        if (keyboard->isKeyDown(OIS::KC_ESCAPE)) {
            break;
        }
#endif

        Quaternion rotationX, rotationY;
        rotationX.MakeFromAngleAxis(-PI_OVER_2, Vector3::RIGHT);
        rotationY.MakeFromAngleAxis(teapotAngle, Vector3::LOOK);
        teapotAngle += 0.0005f;

        Quaternion orientation = rotationX * rotationY;
        Matrix4x4 modelMatrix;
        orientation.ToRotationMatrix(modelMatrix);

        DrawConstants_t teapotDrawConstants;
        teapotDrawConstants.modelMatrix = modelMatrix;

        void* data = drawConstants->Map(MapFlag_t::WRITE_DISCARD);
        memcpy(data, &teapotDrawConstants, sizeof(DrawConstants_t));
        drawConstants->Unmap();

        // Draw the teapot in a render target
        renderDevice->SetRenderTargets(firstPassRenderTargets);

        renderDevice->ClearRenderTargets(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
        renderDevice->ClearDepthStencil(true, false, 1.0f, 0);

        renderDevice->SetVertexShader(vertexShader);
        renderDevice->SetVertexShaderConstantBuffer(passConstants, 0);
        renderDevice->SetVertexShaderConstantBuffer(drawConstants, 1);

        renderDevice->SetFragmentShader(fragmentShader);

        for (const shared_ptr<SubMesh>& subMesh : subMeshes)
        {
            const shared_ptr<VertexBuffer>& vertexBuffer = subMesh->GetVertexBuffer();
            const shared_ptr<IndexBuffer>& indexBuffer = subMesh->GetIndexBuffer();

            renderDevice->DrawIndexed(PrimitiveTopology_t::TRIANGLELIST, vertexBuffer, indexBuffer, 0, 0);
        }

        // Draw a white teapot in a render target
        renderDevice->SetRenderTargets(simplePassRenderTargets);

        renderDevice->ClearRenderTargets(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
        renderDevice->ClearDepthStencil(true, false, 1.0f, 0);

        renderDevice->SetFragmentShader(simplePassShader);

        for (const shared_ptr<SubMesh>& subMesh : subMeshes)
        {
            const shared_ptr<VertexBuffer>& vertexBuffer = subMesh->GetVertexBuffer();
            const shared_ptr<IndexBuffer>& indexBuffer = subMesh->GetIndexBuffer();

            renderDevice->DrawIndexed(PrimitiveTopology_t::TRIANGLELIST, vertexBuffer, indexBuffer, 0, 0);
        }

        // Apply the sobel filter to the teapot
        renderDevice->SetRenderTargets(sobelFilterRenderTargets);

        renderDevice->ClearRenderTargets(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
        renderDevice->ClearDepthStencil(true, false, 1.0f, 0);

        renderDevice->SetVertexShader(fullScreenQuadShader);
        renderDevice->SetFragmentShader(sobelFilterShader);
        renderDevice->SetFragmentShaderConstantBuffer(screenParameters, 0);
        renderDevice->SetFragmentShaderTexture(simplePassRenderTarget, 0);
        renderDevice->SetFragmentShaderSamplerState(samplerState, 0);

        renderDevice->DrawIndexed(PrimitiveTopology_t::TRIANGLELIST, fullScreenQuadVertices, fullScreenQuadIndices, 0, 0);
        
        // Finally, add the two images
        renderDevice->SetDefaultRenderTargetAndDepthStencilBuffer();

        renderDevice->ClearRenderTargets(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
        renderDevice->ClearDepthStencil(true, false, 1.0f, 0);

        renderDevice->SetFragmentShader(imageMultiplicationShader);
        renderDevice->SetFragmentShaderTexture(renderTarget, 0);
        renderDevice->SetFragmentShaderTexture(sobelFilterResult, 1);
        renderDevice->SetFragmentShaderSamplerState(samplerState, 0);

        renderDevice->DrawIndexed(PrimitiveTopology_t::TRIANGLELIST, fullScreenQuadVertices, fullScreenQuadIndices, 0, 0);

        renderContext->SwapBuffers();
    }

    return 0;
}