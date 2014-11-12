#include <math/Vector2.h>
#include <math/Vector3.h>

#include <render/Material.h>
#include <render/Mesh.h>
#include <render/Node.h>
#include <render/Renderer.h>
#include <render/RenderTexture.h>
#include <render/SceneTree.h>
#include <render/Shader.h>
#include <render/Texture2D.h>

#include <system/Logger.h>
#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <string>
#include <vector>
using namespace std;

#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    Window window("Sample_DeferredShading", 1024, 768, true);
    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    // Load the meshes
    Mesh sphereMesh("Media/sphere.obj", true, true);
    Mesh spiderMesh("Media/spider.obj", true, true);

    // Create a plane mesh
    SurfaceTriangles_t surface;
    surface.numVertices = 4;
    surface.numIndices = 6;
    surface.vertices = new Vector3[surface.numVertices];
    surface.indices = new unsigned short[surface.numIndices];

    surface.vertices[0] = Vector3(-1.0f, -1.0f, 0.0f); surface.vertices[1] = Vector3(-1.0f, 1.0f, 0.0f); surface.vertices[2] = Vector3(1.0f, 1.0f, 0.0f); surface.vertices[3] = Vector3(1.0f, -1.0f, 0.0f);
    surface.indices[0] = 0; surface.indices[1] = 3; surface.indices[2] = 2; surface.indices[3] = 0; surface.indices[4] = 2; surface.indices[5] = 1;

    ModelSurface_t modelSurface;
    modelSurface.geometry = &surface;

    Mesh planeMesh;
    planeMesh.AddSurface(modelSurface);
    planeMesh.Initialize();

    // Create the materials
    vector<string> vertexInputs_record;
    vertexInputs_record.push_back("in_vertex");
    vertexInputs_record.push_back("in_normal");
    vertexInputs_record.push_back("in_uv");
    Shader* recordShader = Renderer::GetInstance()->CreateShader("Shaders/record_vert.glsl", "Shaders/record_frag.glsl", vertexInputs_record);
    Material recordMaterial(recordShader);

    vector<string> vertexInputs;
    vertexInputs.push_back("in_vertex");
    vertexInputs.push_back("in_uv");
    Shader* shader = Renderer::GetInstance()->CreateShader("Shaders/vert.glsl", "Shaders/frag.glsl", vertexInputs);
    Material material(shader);

    // Create the nodes
    Node spiderNode;
    spiderNode.SetMaterial(&recordMaterial);
    spiderNode.SetMesh(&spiderMesh);
    spiderNode.SetPosition(Vector3(0.0f, 0.0f, 200.0f));
    Renderer::GetInstance()->GetSceneTree().AddChildren(&spiderNode);

    Node fullscreenQuad;
    fullscreenQuad.SetMaterial(&material);
    fullscreenQuad.SetMesh(&planeMesh);

    // Create the textures for the render buffer
    Texture2D* positionsTexture = Renderer::GetInstance()->CreateTexture2D();
    positionsTexture->SetWidth(1024);
    positionsTexture->SetHeight(768);
    positionsTexture->SetTextureFormat(TEXTURE_FORMAT_RGB32F);
    positionsTexture->SetFilterMode(FILTER_MODE_NEAREST);
    if (!positionsTexture->Create()) {
        Logger::GetInstance()->Error("Couldn't create positions texture");
        return 1;
    }

    Texture2D* normalsTexture = Renderer::GetInstance()->CreateTexture2D();
    normalsTexture->SetWidth(1024);
    normalsTexture->SetHeight(768);
    normalsTexture->SetTextureFormat(TEXTURE_FORMAT_RGB32F);
    normalsTexture->SetFilterMode(FILTER_MODE_NEAREST);
    if (!normalsTexture->Create()) {
        Logger::GetInstance()->Error("Couldn't create normals texture");
        return 1;
    }

    Texture2D* albedosTexture = Renderer::GetInstance()->CreateTexture2D();
    albedosTexture->SetWidth(1024);
    albedosTexture->SetHeight(768);
    albedosTexture->SetTextureFormat(TEXTURE_FORMAT_RGB32F);
    albedosTexture->SetFilterMode(FILTER_MODE_NEAREST);
    if (!albedosTexture->Create()) {
        Logger::GetInstance()->Error("Couldn't create albedos texture");
        return 1;
    }

    // Create the render buffer
    RenderTexture* GBuffer = Renderer::GetInstance()->CreateRenderTexture(1024, 768, TEXTURE_FORMAT_RGB32F);
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
    material.AddTexture("positions", positionsTexture);
    material.AddTexture("normals", normalsTexture);
    material.AddTexture("albedos", albedosTexture);

    float angle = 0.0f;
    while (window.IsOpen()) {
        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

        spiderNode.Yaw(angle);
        angle += 0.001f;

        // Render in the GBuffer
        GBuffer->Bind();
        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->Render();

        // Draw the fullscreen quad using the GBuffer
        Renderer::GetInstance()->BindScreenBuffer();
        Renderer::GetInstance()->Clear();
        fullscreenQuad.ImmediateRender();

        Renderer::GetInstance()->EndRender();
    }

    return 0;
}