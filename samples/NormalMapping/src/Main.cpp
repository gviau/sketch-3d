#include <math/Vector3.h>
#include <render/Material.h>
#include <render/Mesh.h>
#include <render/Node.h>
#include <render/Renderer.h>
#include <render/SceneTree.h>
#include <render/Shader.h>
#include <render/Texture2D.h>
#include <system/Window.h>
using namespace Sketch3D;

#include <string>
#include <vector>
using namespace std;

#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    Window window("Sample_NormalMapping", 1024, 768, true);
    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    // Load the sphere's mesh and generate the tangents
    Mesh sphereMesh("Media/sphere.obj", true, true, true);

    // Create the material for the sphere
    vector<string> vertexInputs;
    vertexInputs.push_back("in_vertex");
    vertexInputs.push_back("in_normal");
    vertexInputs.push_back("in_uv");
    vertexInputs.push_back("in_tangent");
    Shader* shader = Renderer::GetInstance()->CreateShader("Shaders/vert.glsl", "Shaders/frag.glsl", vertexInputs);
    Material sphereMaterial(shader);

    // Add the diffuse and normal texture
    Texture2D* diffuseTexture = Renderer::GetInstance()->CreateTexture2DFromFile("Media/rock_wall.jpg");
    Texture2D* normalTexture = Renderer::GetInstance()->CreateTexture2DFromFile("Media/rock_wall_normal.jpg");
    sphereMaterial.AddTexture("diffuseTexture", diffuseTexture);
    sphereMaterial.AddTexture("normalTexture", normalTexture);

    // Create the sphere node
    Node sphereNode;
    sphereNode.SetMaterial(&sphereMaterial);
    sphereNode.SetMesh(&sphereMesh);
    Renderer::GetInstance()->GetSceneTree().AddChildren(&sphereNode);

    Renderer::GetInstance()->CameraLookAt(Vector3(0.0f, 0.0f, -3.0f), Vector3::ZERO);

    MSG msg;
    while (window.IsOpen()) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->Render();
        Renderer::GetInstance()->EndRender();
    }

    return msg.wParam;
}