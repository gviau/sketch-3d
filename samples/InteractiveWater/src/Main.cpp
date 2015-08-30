#include <framework/Mesh.h>

#include <math/Matrix4x4.h>
#include <math/Vector4.h>

#include <render/Buffer.h>
#include <render/ConstantBuffers.h>
#include <render/HardwareResourceCreator.h>
#include <render/RenderContext.h>
#include <render/RenderDevice.h>
#include <render/Shader.h>

#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <string>
#include <vector>
using namespace std;

#if PLATFORM == PLATFORM_WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif
    Window window("Test", 1024, 768, true);

    RenderParameters_t renderParameters;
    renderParameters.width = 1024;
    renderParameters.height = 768;
    renderParameters.displayFormat = DisplayFormat_t::A8R8G8B8;
    renderParameters.depthStencilBits = DepthStencilBits_t::D32;
    renderParameters.refreshRate = 0;

    shared_ptr<RenderContext> renderContext = CreateRenderContext(RenderSystem_t::DIRECT3D11);
    bool init = renderContext->Initialize(window, renderParameters);

    shared_ptr<RenderDevice> renderDevice = CreateRenderDevice(RenderSystem_t::DIRECT3D11);
    init = renderDevice->Initialize(renderContext);

    HardwareResourceCreator* hardwareResourceCreator = renderDevice->GetHardwareResourceCreator();

    shared_ptr<Mesh> spiderMesh;
    init = LoadMeshFromFile("Media/spider.obj", hardwareResourceCreator, spiderMesh);

    shared_ptr<ConstantBuffer> drawConstantBuffer = hardwareResourceCreator->CreateConstantBuffer();
    init = drawConstantBuffer->Initialize(nullptr, true, false, sizeof(DrawConstants_t));

    shared_ptr<FragmentShader> fragmentShader = hardwareResourceCreator->CreateFragmentShader();
    init = fragmentShader->InitializeFromFile("InteractiveWater/Shaders/frag.hlsl");

    shared_ptr<VertexShader> vertexShader = hardwareResourceCreator->CreateVertexShader();
    init = vertexShader->InitializeFromFile("InteractiveWater/Shaders/vert.hlsl");

    Matrix4x4 modelMatrix;
    float angle = 0.0f;

    renderDevice->SetFragmentShader(fragmentShader);
    renderDevice->SetVertexShader(vertexShader);

    Matrix4x4 projectionMatrix = renderDevice->CalculatePerspectiveProjectionFOV(60.0f, 1024.0f / 768.0f, 1.0f, 1000.0f);

    while (window.IsOpen())
    {
        WindowEvent event;
        if (window.PollEvents(event))
        {
        }

        Matrix4x4 rotX, rotY;
        rotX.RotationAroundX(angle / 2.0f);
        rotY.RotationAroundY(angle);
        modelMatrix = rotY;
        angle += 0.0005f;

        Matrix4x4 translation;
        translation.SetTranslation(Vector3(0.0f, 0.0f, -125.0f));

        void* data = drawConstantBuffer->Map(MapFlag_t::WRITE_DISCARD);
        DrawConstants_t* drawConstants = (DrawConstants_t*)data;
        drawConstants->modelMatrix = projectionMatrix * translation * modelMatrix;
        drawConstantBuffer->Unmap();

        renderDevice->ClearRenderTargets(Vector4(0.1f, 0.1f, 0.1f));
        renderDevice->ClearDepthStencil(true, false, 1.0f, 0);

        renderDevice->SetVertexShaderConstantBuffer(drawConstantBuffer, 1);

        spiderMesh->Draw(renderDevice);

        renderContext->SwapBuffers();
    }

    return 0;
}
