#include <math/Vector2.h>
#include <math/Vector3.h>

#include <framework/Camera.h>

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

#include <memory>
#include <time.h>
#include <string>
#include <vector>
using namespace std;

struct ALIGNED_(16) BurningPaperConstants_t
{
    Vector4 thresholds;
};

#if PLATFORM == PLATFORM_WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif
    Window window("Sample_BurningPaper", 1024, 768, true);
    RenderParameters_t renderParameters;
    renderParameters.width = 1024;
    renderParameters.height = 768;
    renderParameters.displayFormat = DisplayFormat_t::A8R8G8B8;
    renderParameters.refreshRate = 0;
    renderParameters.depthStencilBits = DepthStencilBits_t::D32;

    shared_ptr<RenderContext> renderContext = CreateRenderContext(RenderSystem_t::DIRECT3D11);
    renderContext->Initialize(window, renderParameters);

    shared_ptr<RenderDevice> renderDevice = CreateRenderDevice(RenderSystem_t::DIRECT3D11);
    renderDevice->Initialize(renderContext);

    // Create the paper surface
    Vertex_Pos_UV_t vertices[4];

    vertices[0].position = Vector3(-1.2f, -1.5f, 1.0f); vertices[1].position = Vector3(-1.2f, 1.5f, 1.0f); vertices[2].position = Vector3(1.2f, 1.5f, 1.0f); vertices[3].position = Vector3(1.2f, -1.5f, 1.0f);
    vertices[0].uv = Vector2(0.0f, 0.0f); vertices[1].uv = Vector2(0.0f, 1.0f); vertices[2].uv = Vector2(1.0f, 1.0f); vertices[3].uv = Vector2(1.0f, 0.0f);

    unsigned short indices[6];
    indices[0] = 0; indices[1] = 2; indices[2] = 1; indices[3] = 0; indices[4] = 3; indices[5] = 2;

    shared_ptr<VertexBuffer> vertexBuffer = renderDevice->GetHardwareResourceCreator()->CreateVertexBuffer();
    shared_ptr<IndexBuffer> indexBuffer = renderDevice->GetHardwareResourceCreator()->CreateIndexBuffer();
    vertexBuffer->Initialize(vertices, false, false, VertexFormatType_t::Pos_UV, 4);
    indexBuffer->Initialize(indices, false, false, IndexFormat_t::INT_2_BYTES, 6);

    // Create the paper material
    shared_ptr<VertexShader> vertexShader = renderDevice->GetHardwareResourceCreator()->CreateVertexShader();
    shared_ptr<FragmentShader> fragmentShader = renderDevice->GetHardwareResourceCreator()->CreateFragmentShader();
    vertexShader->InitializeFromFile("Shaders/BurningPaper/vert.hlsl");
    fragmentShader->InitializeFromFile("Shaders/BurningPaper/frag.hlsl");

    shared_ptr<TextureMap> paperTextureMap;
    shared_ptr<TextureMap> noiseTextureMap;

    TextureFormat_t paperTextureFormat;
    TextureFormat_t noiseTextureFormat;

    LoadTextureMapFromFile("Media/paper.jpg", paperTextureMap, paperTextureFormat);
    LoadTextureMapFromFile("Media/noise.jpg", noiseTextureMap, noiseTextureFormat);

    shared_ptr<Texture2D> paperTexture = renderDevice->GetHardwareResourceCreator()->CreateTexture2D();
    shared_ptr<Texture2D> noiseTexture = renderDevice->GetHardwareResourceCreator()->CreateTexture2D();
    paperTexture->Initialize(paperTextureMap.get(), paperTextureFormat, false);
    noiseTexture->Initialize(noiseTextureMap.get(), noiseTextureFormat, false);

    Camera camera;
    camera.LookAt(Vector3(0.0f, 0.0f, 2.0f), -Vector3::LOOK, Vector3::UP);

    float threshold = 0.0f;
    float range = 0.075f;
    float t = 0.0f;
    clock_t begin, end;

    /*
#if PLATFORM == PLATFORM_WIN32
    Text::GetInstance()->SetTextFont("C:/Windows/Fonts/Arial.ttf");
#elif PLATFORM == PLATFORM_LINUX
    Text::GetInstance()->SetTextFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
#endif

    Text::GetInstance()->SetTextSize(24, 24);
    */

    renderDevice->SetVertexShader(vertexShader);
    renderDevice->SetFragmentShader(fragmentShader);

    renderDevice->SetFragmentShaderTexture(paperTexture, 0);
    renderDevice->SetFragmentShaderTexture(noiseTexture, 1);

    shared_ptr<ConstantBuffer> burningPaperConstants = renderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();
    burningPaperConstants->Initialize(nullptr, true, false, sizeof(BurningPaperConstants_t));

    renderDevice->SetFragmentShaderConstantBuffer(burningPaperConstants, 0);

    shared_ptr<ConstantBuffer> drawConstants = renderDevice->GetHardwareResourceCreator()->CreateConstantBuffer();

    DrawConstants_t initData;
    initData.modelViewProjectionMatrix = renderDevice->CalculatePerspectiveProjectionFOV(60.0f, 1024.0f / 768.0f, 1.0f, 1000.0f) * camera.GetViewMatrix();

    drawConstants->Initialize(&initData, false, false, sizeof(DrawConstants_t));

    shared_ptr<SamplerState> samplerState = renderDevice->GetHardwareResourceCreator()->CreateSamplerState();
    samplerState->Initialize(FilterMode_t::LINEAR, AddressMode_t::CLAMP, AddressMode_t::CLAMP, AddressMode_t::CLAMP, ComparisonFunction_t::ALWAYS);

    renderDevice->SetVertexShaderConstantBuffer(drawConstants, 0);
    renderDevice->SetFragmentShaderSamplerState(samplerState, 0);

    float fps = 0;
    float dt = 0.0f;
    size_t numFrames = 0;

    while (window.IsOpen()) {
        begin = clock();

        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

        threshold += t / 3.0f;
        
        void* data = burningPaperConstants->Map(MapFlag_t::WRITE_DISCARD);

        ((BurningPaperConstants_t*)data)->thresholds = Vector4(threshold, range, 0.0f, 0.0f);

        burningPaperConstants->Unmap();

        renderDevice->SetFragmentShaderConstantBuffer(burningPaperConstants, 1);

        renderDevice->ClearRenderTargets(Vector4(0.2f, 0.2f, 0.2f, 1.0f));
        renderDevice->ClearDepthStencil(true, false, 1.0f, 0);

        renderDevice->DrawIndexed(PrimitiveTopology_t::TRIANGLELIST, vertexBuffer, indexBuffer, 0, 0);

        renderContext->SwapBuffers();

        // Text::GetInstance()->Write(to_string(fps), 5, 5);

        end = clock();
        t = float(end - begin) / CLOCKS_PER_SEC;
        dt += float(end - begin) / CLOCKS_PER_SEC;

        numFrames += 1;
        if (dt >= 0.25f) {
            fps = (float)numFrames / dt;
            numFrames = 0;
            dt -= 0.25f;
        }
    }

    return 0;
}
