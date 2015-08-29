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

    shared_ptr<RenderContext> renderContext = CreateRenderContext(RenderSystem_t::OPENGL);
    bool init = renderContext->Initialize(window, renderParameters);

    shared_ptr<RenderDevice> renderDevice = CreateRenderDevice(RenderSystem_t::OPENGL);
    init = renderDevice->Initialize(renderContext);

    HardwareResourceCreator* hardwareResourceCreator = renderDevice->GetHardwareResourceCreator();

    Vertex_Pos_UV_t vertices[] = {
        { Vector3(-0.2f, -0.2f, 0.2f), Vector2(0.0f, 0.0f) },
        { Vector3(-0.2f,  0.2f, 0.2f), Vector2(0.0f, 1.0f) },
        { Vector3( 0.2f,  0.2f, 0.2f), Vector2(1.0f, 1.0f) },
        { Vector3( 0.2f, -0.2f, 0.2f), Vector2(1.0f, 0.0f) },
        { Vector3(-0.2f, -0.2f, 0.6f), Vector2(1.0f, 0.0f) },
        { Vector3(-0.2f,  0.2f, 0.6f), Vector2(1.0f, 1.0f) },
        { Vector3( 0.2f,  0.2f, 0.6f), Vector2(0.0f, 1.0f) },
        { Vector3( 0.2f, -0.2f, 0.6f), Vector2(0.0f, 0.0f) }
    };
    size_t numVertices = _countof(vertices);

    VertexFormat_Pos_UV vertexFormat;
    shared_ptr<VertexBuffer> vertexBuffer = hardwareResourceCreator->CreateVertexBuffer();
    init = vertexBuffer->Initialize((void*)vertices, false, false, &vertexFormat, numVertices);

    unsigned short indices[] = {
		0, 1, 2, 0, 2, 3,
        3, 2, 6, 3, 6, 7,
        7, 6, 5, 7, 5, 4,
        4, 5, 1, 4, 1, 0,
        1, 5, 6, 1, 6, 2,
        4, 0, 3, 4, 3, 7
    };
    size_t numIndices = _countof(indices);

    shared_ptr<IndexBuffer> indexBuffer = hardwareResourceCreator->CreateIndexBuffer();
    init = indexBuffer->Initialize((void*)indices, false, false, IndexFormat_t::INT_2_BYTES, numIndices);

    shared_ptr<ConstantBuffer> drawConstantBuffer = hardwareResourceCreator->CreateConstantBuffer();
    init = drawConstantBuffer->Initialize(nullptr, true, false, sizeof(DrawConstants_t));

    shared_ptr<FragmentShader> fragmentShader = hardwareResourceCreator->CreateFragmentShader();
    init = fragmentShader->InitializeFromFile("InteractiveWater/Shaders/frag.glsl");

    shared_ptr<VertexShader> vertexShader = hardwareResourceCreator->CreateVertexShader();
    init = vertexShader->InitializeFromFile("InteractiveWater/Shaders/vert.glsl");
    init = vertexShader->CreateInputLayout(&vertexFormat);

    Matrix4x4 modelMatrix;
    float angle = 0.0f;

    renderDevice->SetFragmentShader(fragmentShader);
    renderDevice->SetVertexShader(vertexShader);

    while (window.IsOpen())
    {
        WindowEvent event;
        if (window.PollEvents(event))
        {
        }

        Matrix4x4 rotX, rotY;
        rotX.RotationAroundX(angle / 2.0f);
        rotY.RotationAroundY(angle);
        modelMatrix = rotX * rotY;
        angle += 0.001f;
        modelMatrix = modelMatrix;

        void* data = drawConstantBuffer->Map(MapFlag_t::WRITE_DISCARD);
        DrawConstants_t* drawConstants = (DrawConstants_t*)data;
        drawConstants->modelMatrix = modelMatrix;
        drawConstantBuffer->Unmap();

        renderDevice->ClearRenderTargets(Vector4(0.1f, 0.1f, 0.1f));
        renderDevice->ClearDepthStencil(true, false, 1.0f, 0);

        renderDevice->SetVertexShaderConstantBuffer(drawConstantBuffer, 0);

        renderDevice->DrawIndexed(PrimitiveTopology_t::TRIANGLELIST, vertexBuffer, indexBuffer, 0, 0);

        renderContext->SwapBuffers();
    }

    return 0;
}
