#include <math/Matrix4x4.h>
#include <math/Vector2.h>
#include <math/Vector4.h>

#include <render/Buffer.h>
#include <render/ConstantBuffers.h>
#include <render/HardwareResourceCreator.h>
#include <render/RenderContext.h>
#include <render/RenderDevice.h>
#include <render/RenderTarget.h>
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
        { Vector3(-1.0f, -1.0f,  1.0f), Vector2(1.0f, 0.0f) },
        { Vector3(-1.0f,  1.0f,  1.0f), Vector2(1.0f, 1.0f) },
        { Vector3( 1.0f,  1.0f,  1.0f), Vector2(0.0f, 1.0f) },
        { Vector3( 1.0f, -1.0f,  1.0f), Vector2(0.0f, 0.0f) }
    };
    size_t numVertices = _countof(vertices);

    VertexFormat_Pos_UV vertexFormat;
    shared_ptr<VertexBuffer> vertexBuffer = hardwareResourceCreator->CreateVertexBuffer();
    bool init = vertexBuffer->Initialize((void*)vertices, false, false, &vertexFormat, numVertices);

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

    Vertex_Pos_UV_t planeVertices[] = {
        { Vector3(-5.0f, 0.0f, -5.0f), Vector2(0.0f, 0.0f) },
        { Vector3(-5.0f, 0.0f,  5.0f), Vector2(0.0f, 1.0f) },
        { Vector3( 5.0f, 0.0f,  5.0f), Vector2(1.0f, 1.0f) },
        { Vector3( 5.0f, 0.0f, -5.0f), Vector2(1.0f, 0.0f) }
    };
    numVertices = _countof(planeVertices);

    shared_ptr<VertexBuffer> planeVertexBuffer = hardwareResourceCreator->CreateVertexBuffer();
    init = planeVertexBuffer->Initialize((void*)planeVertices, false, false, &vertexFormat, numVertices);

    unsigned short planeIndices[] = {
        0, 1, 2,
        0, 2, 3
    };
    numIndices = _countof(planeIndices);

    shared_ptr<IndexBuffer> planeIndexBuffer = hardwareResourceCreator->CreateIndexBuffer();
    init = planeIndexBuffer->Initialize((void*)planeIndices, false, false, IndexFormat_t::INT_2_BYTES, numIndices);

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

    Vector3 position(-4.0f, 2.0f, -4.0f);
    Vector3 direction = Vector3::ZERO - position;
	direction.Normalize();

    Vector3 r = direction.Cross(Vector3::UP);
	r.Normalize();

	Vector3 u = r.Cross(direction);
	u.Normalize();

    Matrix4x4 view;
    view[0][0] = r.x;
    view[0][1] = r.y;
    view[0][2] = r.z;

    view[1][0] = u.x;
    view[1][1] = u.y;
    view[1][2] = u.z;

    view[2][0] = -direction.x;
    view[2][1] = -direction.y;
    view[2][2] = -direction.z;

    view[0][3] = -position.Dot(r);
    view[1][3] = -position.Dot(u);
    view[2][3] =  position.Dot(direction);

	height = tan(75.0f * DEG_2_RAD_OVER_2);
	width = height * 1024.0f / 768.0f;
    right = width;
    left = -width;
    top = height;
    bottom = -height;
    nearPlane = 1.0f;
    farPlane = 1000.0f;

    w = right - left;
    h = top - bottom;
    dz = nearPlane - farPlane;
    z2 = 2.0f * nearPlane;

    Matrix4x4 lightProjectionMatrix;
    projection[0][0] = z2 / w;
    projection[1][1] = z2 / h;
    projection[2][2] = farPlane / dz;
    projection[3][2] = -1.0f;
    projection[2][3] = nearPlane * farPlane / dz;
    projection[3][3] = 0.0f;

    position = Vector3(0.0f, 5.0f, -10.0f);
    direction = Vector3::ZERO - position;
	direction.Normalize();

    r = direction.Cross(Vector3::UP);
	r.Normalize();

	u = r.Cross(direction);
	u.Normalize();

    Matrix4x4 lightViewMatrix;
    lightViewMatrix[0][0] = r.x;
    lightViewMatrix[0][1] = r.y;
    lightViewMatrix[0][2] = r.z;

    lightViewMatrix[1][0] = u.x;
    lightViewMatrix[1][1] = u.y;
    lightViewMatrix[1][2] = u.z;

    lightViewMatrix[2][0] = -direction.x;
    lightViewMatrix[2][1] = -direction.y;
    lightViewMatrix[2][2] = -direction.z;

    lightViewMatrix[0][3] = -position.Dot(r);
    lightViewMatrix[1][3] = -position.Dot(u);
    lightViewMatrix[2][3] =  position.Dot(direction);

    PassConstants_t initialPassConstants;
    initialPassConstants.projectionMatrix = projection.Transpose();
    initialPassConstants.viewMatrix = view.Transpose();
    shared_ptr<ConstantBuffer> constantBuffer = hardwareResourceCreator->CreateConstantBuffer();
    init = constantBuffer->Initialize((void*)&initialPassConstants, true, false, sizeof(PassConstants_t));

    shared_ptr<ConstantBuffer> drawConstantBuffer = hardwareResourceCreator->CreateConstantBuffer();
    init = drawConstantBuffer->Initialize(nullptr, true, false, sizeof(DrawConstants_t));

    ShadowConstants_t initialShadowConstants;
    initialShadowConstants.lightProjectionMatrix = lightProjectionMatrix;
    initialShadowConstants.lightViewMatrix = lightViewMatrix;
    shared_ptr<ConstantBuffer> shadowConstantBuffer = hardwareResourceCreator->CreateConstantBuffer();
    init = shadowConstantBuffer->Initialize((void*)&initialShadowConstants, true, false, sizeof(ShadowConstants_t));

    shared_ptr<FragmentShader> fragmentShader = hardwareResourceCreator->CreateFragmentShader();
    init = fragmentShader->InitializeFromFile("InteractiveWater/Shaders/frag.hlsl");

    shared_ptr<VertexShader> vertexShader = hardwareResourceCreator->CreateVertexShader();
    init = vertexShader->InitializeFromFile("InteractiveWater/Shaders/vert.hlsl");
    init = vertexShader->CreateInputLayout(&vertexFormat);

    shared_ptr<VertexShader> drawShadowShader = hardwareResourceCreator->CreateVertexShader();
    init = drawShadowShader->InitializeFromFile("InteractiveWater/Shaders/drawShadow.hlsl");
    init = drawShadowShader->CreateInputLayout(&vertexFormat);

    shared_ptr<FragmentShader> recordShadowShader = hardwareResourceCreator->CreateFragmentShader();
    init = recordShadowShader->InitializeFromFile("InteractiveWater/Shaders/recordShadow.hlsl");

    shared_ptr<FragmentShader> drawShadow = hardwareResourceCreator->CreateFragmentShader();
    init = drawShadow->InitializeFromFile("InteractiveWater/Shaders/shadowFrag.hlsl");

    shared_ptr<SamplerState> samplerState = hardwareResourceCreator->CreateSamplerState();
    init = samplerState->Initialize(FilterMode_t::NEAREST, AddressMode_t::CLAMP, AddressMode_t::CLAMP, AddressMode_t::CLAMP, ComparisonFunction_t::ALWAYS);

    unsigned char* data1 = new unsigned char[16];
    data1[0] = 0xFFFFFFFF;
    data1[4] = 0x00000000;
    data1[8] = 0x00000000;
    data1[12] = 0xFFFFFFFF;
    TextureMap textureMap1(data1, 2, 2);

    shared_ptr<Texture2D> squares = hardwareResourceCreator->CreateTexture2D();
    init = squares->Initialize(&textureMap1, TextureFormat_t::RGBA32, false, false);

    unsigned char* data2 = new unsigned char[16];
    memset(data2, 255, 16);
    TextureMap textureMap2(data2, 2, 2);

    shared_ptr<Texture2D> white = hardwareResourceCreator->CreateTexture2D();
    init = white->Initialize(&textureMap2, TextureFormat_t::RGBA32, false, false);

    shared_ptr<TextureMap> loadedTextureMap;
    init = LoadTextureMapFromFile("InteractiveWater/Media/water.jpg", loadedTextureMap);    

    shared_ptr<Texture2D> loadedTexture = hardwareResourceCreator->CreateTexture2D();
    init = loadedTexture->Initialize(loadedTextureMap.get(), TextureFormat_t::RGB24, false);

    shared_ptr<RenderTarget> renderTarget = hardwareResourceCreator->CreateRenderTarget();
    init = renderTarget->Initialize(1024, 768, TextureFormat_t::RGBA32);

    vector<shared_ptr<RenderTarget>> renderTargets;
    renderTargets.push_back(renderTarget);

    shared_ptr<DepthStencilTarget> depthStencilTarget = hardwareResourceCreator->CreateDepthStencilTarget();
    init = depthStencilTarget->Initialize(1024, 768, DepthStencilBits_t::D32);

    DrawConstants_t* drawConstants;
    Matrix4x4 modelMatrix;
    modelMatrix.SetTranslation(Vector3(2.0f, 1.0f, 1.0f));
    float angle = 0.0f;

    vector<shared_ptr<RenderTarget>> noRenderTargets;
    RasterizerState_t shadowRecordState = renderDevice->GetDefaultRasterizerState();
    shadowRecordState.cullingMethod = CullingMethod_t::FRONT_FACE;

    Matrix4x4 modelViewProjection = projection * view * modelMatrix;

    while (window.IsOpen()) {
        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

        renderDevice->ClearRenderTargets(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
        renderDevice->ClearDepthStencil(true, false, 1.0f, 0);

        void* data = drawConstantBuffer->Map(MapFlag_t::WRITE_DISCARD);
        drawConstants = (DrawConstants_t*)data;
        drawConstants->modelMatrix = Matrix4x4::IDENTITY;
        drawConstantBuffer->Unmap();

        renderDevice->SetVertexShader(vertexShader);
        renderDevice->SetVertexShaderConstantBuffer(constantBuffer, 0);
        renderDevice->SetVertexShaderConstantBuffer(drawConstantBuffer, 1);

        renderDevice->SetFragmentShader(fragmentShader);
        renderDevice->SetFragmentShaderSamplerState(samplerState, 0);
        renderDevice->SetFragmentShaderTexture(loadedTexture, 0);

        renderDevice->DrawIndexed(PrimitiveTopology_t::TRIANGLELIST, vertexBuffer, indexBuffer, 0, 0);

        renderContext->SwapBuffers();
    }

    return 0;
}
