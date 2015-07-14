#include <math/Matrix4x4.h>
#include <math/Vector2.h>
#include <math/Vector4.h>

#include <render/Buffer.h>
#include <render/ConstantBuffers.h>
#include <render/HardwareResourceCreator.h>
#include <render/RenderContext.h>
#include <render/RenderDevice.h>
#include <render/SamplerState.h>
#include <render/Shader.h>
#include <render/Texture.h>
#include <render/TextureMap.h>
#include <render/VertexFormat.h>

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
    renderContext->Initialize(window, renderParameters);

    shared_ptr<RenderDevice> renderDevice = CreateRenderDevice(RenderSystem_t::DIRECT3D11);
    renderDevice->Initialize(renderContext);

    HardwareResourceCreator* hardwareResourceCreator = renderDevice->GetHardwareResourceCreator();

    Vertex_Pos_UV_t vertices[] = {
        { Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0f, 0.0f) },
        { Vector3(-1.0f,  1.0f, -1.0f), Vector2(0.0f, 1.0f) },
        { Vector3( 1.0f,  1.0f, -1.0f), Vector2(1.0f, 1.0f) },
        { Vector3( 1.0f, -1.0f, -1.0f), Vector2(1.0f, 0.0f) },
        { Vector3(-1.0f, -1.0f,  1.0f), Vector2(0.0f, 0.0f) },
        { Vector3(-1.0f,  1.0f,  1.0f), Vector2(0.0f, 1.0f) },
        { Vector3( 1.0f,  1.0f,  1.0f), Vector2(1.0f, 1.0f) },
        { Vector3( 1.0f, -1.0f,  1.0f), Vector2(1.0f, 0.0f) }
    };
    size_t numVertices = _countof(vertices);

    VertexFormat_Pos_UV vertexFormat;
    shared_ptr<VertexBuffer> vertexBuffer = hardwareResourceCreator->CreateVertexBuffer((void*)vertices, false, false, &vertexFormat, numVertices);

    unsigned short indices[] = {
		0, 1, 2, 0, 2, 3,
        3, 2, 6, 3, 6, 7,
        7, 6, 5, 7, 5, 4,
        4, 5, 1, 4, 1, 0,
        1, 5, 6, 1, 6, 2,
        4, 0, 3, 4, 3, 7
    };
    size_t numIndices = _countof(indices);

    shared_ptr<IndexBuffer> indexBuffer = hardwareResourceCreator->CreateIndexBuffer((void*)indices, false, false, IndexFormat_t::INT_2_BYTES, numIndices);

	float height = tan(60.0f * DEG_2_RAD_OVER_2);
	float width = height * 1024.0f / 768.0f;
    float right = width;
    float left = -width;
    float top = height;
    float bottom = -height;
    float nearPlane = 1.0f;
    float farPlane = 1000.0f;

    float w = right - left;
    float h = top - bottom;
    float dz = nearPlane - farPlane;
    float z2 = 2.0f * nearPlane;

    Matrix4x4 projection;
    projection[0][0] = z2 / w;
    projection[1][1] = z2 / h;
    projection[2][2] = farPlane / dz;
    projection[3][2] = -1.0f;
    projection[2][3] = nearPlane * farPlane / dz;
    projection[3][3] = 0.0f;

    Matrix4x4 view;
    view[2][3] = -5.0f;

    PassConstants_t initialPassConstants;
    initialPassConstants.projectionMatrix = projection.Transpose();
    shared_ptr<ConstantBuffer> constantBuffer = hardwareResourceCreator->CreateConstantBuffer((void*)&initialPassConstants, true, false, sizeof(PassConstants_t));

    shared_ptr<FragmentShader> fragmentShader = hardwareResourceCreator->CreateFragmentShader();
    bool init = fragmentShader->InitializeFromFile("InteractiveWater/Shaders/frag.hlsl");

    shared_ptr<VertexShader> vertexShader = hardwareResourceCreator->CreateVertexShader();
    init = vertexShader->InitializeFromFile("InteractiveWater/Shaders/vert.hlsl");
    init = vertexShader->CreateInputLayout(&vertexFormat);

    renderDevice->SetFragmentShader(fragmentShader);
    renderDevice->SetVertexShader(vertexShader);

    shared_ptr<SamplerState> samplerState = hardwareResourceCreator->CreateSamplerState(FilterMode_t::LINEAR, AddressMode_t::CLAMP, AddressMode_t::CLAMP,
                                                                                        AddressMode_t::CLAMP, ComparisonFunction_t::ALWAYS);

    unsigned char data[] = {
        255, 255, 255, 255, 0, 0, 0, 0,
        0, 0, 0, 0, 255, 255, 255, 255
    };
    TextureMap textureMap(data, 2, 2);

    shared_ptr<Texture2D> texture = hardwareResourceCreator->CreateTexture2D(&textureMap, TextureFormat_t::RGBA32, false, false);

    renderDevice->SetFragmentShaderSamplerState(samplerState, 0);
    renderDevice->SetFragmentShaderTexture(texture, 0);

    PassConstants_t* passConstants;
    Matrix4x4 modelMatrix;
    float angle = 0.0f;

    while (window.IsOpen()) {
        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

        renderDevice->ClearRenderTargets(Vector4(0.1f, 0.1f, 0.1f));
        renderDevice->ClearDepthStencil(true, false, 1.0f, 0);
        
        void* data = constantBuffer->Map(MapFlag_t::WRITE_DISCARD);

        passConstants = (PassConstants_t*)data;
        passConstants->projectionMatrix = projection.Transpose();
        passConstants->viewMatrix = view.Transpose();
        passConstants->modelMatrix = modelMatrix.Transpose();

        constantBuffer->Unmap();

        Matrix4x4 rotX, rotY;
        rotX.RotationAroundX(angle);
        rotY.RotationAroundY(angle);
        modelMatrix = rotX * rotY;
        angle += 0.001f;

        renderDevice->SetVertexShaderConstantBuffer(constantBuffer);
        renderDevice->DrawIndexed(PrimitiveTopology_t::TRIANGLELIST, vertexBuffer, indexBuffer, 0, 0);

        renderContext->SwapBuffers();
    }

    return 0;
}
