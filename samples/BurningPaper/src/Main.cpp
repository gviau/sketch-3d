#include <math/Vector2.h>
#include <math/Vector3.h>

#include <render/Material.h>
#include <render/Mesh.h>
#include <render/Node.h>
#include <render/Renderer.h>
#include <render/SceneTree.h>
#include <render/Shader.h>
#include <render/Texture2D.h>

#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <time.h>
#include <string>
#include <vector>
using namespace std;

#if defined(__WIN32__) || defined(_WIN32)
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif
    Window window("Sample_BurningPaper", 1024, 768, true);
    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    // Create the paper surface
    SurfaceTriangles_t surface;
    surface.numVertices = 4;
    surface.numTexCoords = 4;
    surface.numIndices = 6;
    surface.vertices = new Vector3[surface.numVertices];
    surface.texCoords = new Vector2[surface.numTexCoords];
    surface.indices = new unsigned short[surface.numIndices];

    surface.vertices[0] = Vector3(-1.2f, -1.5f, 1.0f); surface.vertices[1] = Vector3(-1.2f, 1.5f, 1.0f); surface.vertices[2] = Vector3(1.2f, 1.5f, 1.0f); surface.vertices[3] = Vector3(1.2f, -1.5f, 1.0f);
    surface.texCoords[0] = Vector2(0.0f, 0.0f); surface.texCoords[1] = Vector2(0.0f, 1.0f); surface.texCoords[2] = Vector2(1.0f, 1.0f); surface.texCoords[3] = Vector2(1.0f, 0.0f);
    surface.indices[0] = 0; surface.indices[1] = 2; surface.indices[2] = 1; surface.indices[3] = 0; surface.indices[4] = 3; surface.indices[5] = 2;

    ModelSurface_t model;
    model.geometry = &surface;
    Mesh paperMesh;
    paperMesh.AddSurface(model);

    VertexAttributesMap_t vertexAttributes;
    vertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    vertexAttributes[VERTEX_ATTRIBUTES_TEX_COORDS] = 1;
    paperMesh.Initialize(vertexAttributes);
    
    // Create the paper material
    vector<string> vertexInputs;
    vertexInputs.push_back("in_vertex");
    vertexInputs.push_back("in_uv");
    Shader* shader = Renderer::GetInstance()->CreateShader("Shaders/BurningPaper/vert.glsl", "Shaders/BurningPaper/frag.glsl", vertexInputs);
    Material paperMaterial(shader);

    Texture2D* paperTexture = Renderer::GetInstance()->CreateTexture2DFromFile("Media/paper.jpg");
    Texture2D* noiseTexture = Renderer::GetInstance()->CreateTexture2DFromFile("Media/noise.jpg");
    paperMaterial.AddTexture("paper", paperTexture);
    paperMaterial.AddTexture("noise", noiseTexture);

    // Create the paper node
    Node paperNode;
    paperNode.SetMaterial(&paperMaterial);
    paperNode.SetMesh(&paperMesh);
    Renderer::GetInstance()->GetSceneTree().AddNode(&paperNode);

    Renderer::GetInstance()->CameraLookAt(Vector3(0.0f, 0.0f, 5.0f), -Vector3::LOOK, Vector3::UP);

    float threshold = 0.0f;
    float range = 0.075f;
    float t = 0.0f;
    clock_t begin, end;

    while (window.IsOpen()) {
        begin = clock();

        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

        //threshold = powf(1.05, t / 5000.0f);
        threshold += t / 7500.0f;
        shader->SetUniformVector2("thresholds", threshold, range);

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->StartRender();
        Renderer::GetInstance()->Render();
        Renderer::GetInstance()->EndRender();

        end = clock();
        t += float(end - begin) / CLOCKS_PER_SEC;
    }

    return 0;
}
