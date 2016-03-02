#include <math/Vector2.h>
#include <math/Vector3.h>

#include <framework/Camera.h>
#include <framework/ForwardRenderingPipeline.h>
#include <framework/Light.h>
#include <framework/Mesh.h>
#include <framework/Scene.h>
#include <framework/VisualNode.h>

#include <render/Buffer.h>
#include <render/ConstantBuffers.h>
#include <render/HardwareResourceCreator.h>
#include <render/RenderContext.h>
#include <render/RenderDevice.h>

#include <system/Logger.h>
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
    Window window("Sample_DeferredShading", 1024, 768, true);
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

    ForwardRenderingPipeline forwardRenderingPipeline(renderContext, renderDevice);
    if (!forwardRenderingPipeline.Initialize())
    {
        Logger::GetInstance()->Error("Couldn't initialize the forward rendering pipeline");
        return 1;
    }

    // Load the mesh
    shared_ptr<Mesh> bobMesh;
    if (!LoadMeshFromFileWithMaterial("Media/Bob.md5mesh", renderDevice, bobMesh, forwardRenderingPipeline.GetMaterialCodeGenerator()))
    {
        Logger::GetInstance()->Error("Couldn't load mesh with material");
        return 1;
    }

    // Create the scene
    Scene scene;

    shared_ptr<VisualNode> bobNode(new VisualNode);
    bobNode->SetMesh(bobMesh);
    bobNode->SetPosition(0.0f, 0.0f, -150.0f);

    scene.GetRootNode().AddChild(bobNode);

    vector<shared_ptr<Light>> lights;

    shared_ptr<Light> light(new Light);
    light->SetLightType(LightType_t::Point);
    light->SetPosition(Vector3(60.0f, 35.0f, -150.0f));
    light->SetColor(Vector4(0.2f, 0.2f, 0.8f));
    lights.push_back(light);

    scene.SetLights(lights);

    Camera camera;

    /*
    // Create a plane mesh
    SurfaceTriangles_t surface;
    surface.numVertices = 4;
    surface.numIndices = 6;
    surface.vertices = new Vector3[surface.numVertices];
    surface.indices = new unsigned short[surface.numIndices];

    surface.vertices[0] = Vector3(-1.0f, -1.0f, 0.0f); surface.vertices[1] = Vector3(-1.0f, 1.0f, 0.0f); surface.vertices[2] = Vector3(1.0f, 1.0f, 0.0f); surface.vertices[3] = Vector3(1.0f, -1.0f, 0.0f);
    surface.indices[0] = 0; surface.indices[1] = 3; surface.indices[2] = 2; surface.indices[3] = 0; surface.indices[4] = 2; surface.indices[5] = 1;

    Mesh planeMesh;
    planeMesh.AddSurface(&surface);

    VertexAttributesMap_t vertexAttributesPlane;
    vertexAttributesPlane[VERTEX_ATTRIBUTES_POSITION] = 0;
    planeMesh.Initialize(vertexAttributesPlane);

    // Create the materials
    Shader* recordShader = Renderer::GetInstance()->CreateShader();
    recordShader->SetSourceFile("Shaders/DeferredShading/record_vert", "Shaders/DeferredShading/record_frag");
    Material recordMaterial(recordShader);

    Shader* shader = Renderer::GetInstance()->CreateShader();
    shader->SetSourceFile("Shaders/DeferredShading/vert", "Shaders/DeferredShading/frag");
    Material material(shader);

    // Create the nodes
    Node spiderNode;
    spiderNode.SetMaterial(&recordMaterial);
    spiderNode.SetMesh(&spiderMesh);
    spiderNode.SetPosition(Vector3(0.0f, 0.0f, 200.0f));
    Renderer::GetInstance()->GetSceneTree().AddNode(&spiderNode);

    Node fullscreenQuad;
    fullscreenQuad.SetMaterial(&material);
    fullscreenQuad.SetMesh(&planeMesh);

    // Create the render buffer
    RenderTexture* GBuffer = Renderer::GetInstance()->CreateRenderTexture(1024, 768, TEXTURE_FORMAT_RGBA32F);

    // Create the textures for the render buffer
    Texture2D* positionsTexture = GBuffer->CreateTexture2D();
    Texture2D* normalsTexture = GBuffer->CreateTexture2D();
    Texture2D* albedosTexture = GBuffer->CreateTexture2D();

    vector<Texture2D*> gbufferTextures;
    gbufferTextures.push_back(positionsTexture);
    gbufferTextures.push_back(normalsTexture);
    gbufferTextures.push_back(albedosTexture);
    if (!GBuffer->AttachTextures(gbufferTextures)) {
        Logger::GetInstance()->Error("Couldn't attach textures to the GBuffer");
        return 1;
    }

    if (!GBuffer->AddDepthBuffer()) {
        Logger::GetInstance()->Error("Couldn't add a depth buffer to the GBuffer");
        return 1;
    }

    // Attach the GBuffer texture to the material that is being used to render on the fullscreen quad
    material.SetUniformTexture("positions", positionsTexture);
    material.SetUniformTexture("normals", normalsTexture);
    material.SetUniformTexture("albedos", albedosTexture);
    */
    bobNode->SetOrientation(0.0f, 0.0f, -PI_OVER_2);

    while (window.IsOpen()) {
        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

        bobNode->Yaw(0.001f);

        forwardRenderingPipeline.RenderSceneFromCamera(camera, scene);

        /*
        spiderNode.Yaw(0.001f);

        // Render in the GBuffer
        GBuffer->Bind();

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->StartRender();
        Renderer::GetInstance()->Render();

        // Draw the fullscreen quad using the GBuffer
        Renderer::GetInstance()->BindScreenBuffer();
        Renderer::GetInstance()->Clear();

        fullscreenQuad.Render();
        Renderer::GetInstance()->EndRender();

        Renderer::GetInstance()->PresentFrame();
        */
    }

    return 0;
}
