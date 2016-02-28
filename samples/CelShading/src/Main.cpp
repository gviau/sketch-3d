#include <math/Quaternion.h>
#include <math/Vector3.h>

#include <framework/Camera.h>
#include <framework/Mesh.h>
#include <framework/SubMesh.h>

#include <render/Buffer.h>
#include <render/ConstantBuffers.h>
#include <render/HardwareResourceCreator.h>
#include <render/RenderContext.h>
#include <render/RenderDevice.h>
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

        renderDevice->ClearRenderTargets(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
        renderDevice->ClearDepthStencil(true, false, 1.0f, 0);

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

        renderContext->SwapBuffers();
    }

    return 0;
}