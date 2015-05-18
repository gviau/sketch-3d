#include <math/Vector2.h>
#include <math/Vector3.h>

#include <render/Material.h>
#include <render/Mesh.h>
#include <render/Node.h>
#include <render/Renderer.h>
#include <render/SceneTree.h>
#include <render/Shader.h>
#include <render/Text.h>
#include <render/Texture2D.h>

#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <time.h>
#include <string>
#include <vector>
using namespace std;

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
    renderParameters.displayFormat = DISPLAY_FORMAT_X8R8G8B8;
    renderParameters.refreshRate = 0;
    renderParameters.depthStencilBits = DEPTH_STENCIL_BITS_D24X8;

    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window, renderParameters);
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

    Mesh paperMesh;
    paperMesh.AddSurface(&surface);

    VertexAttributesMap_t vertexAttributes;
    vertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    vertexAttributes[VERTEX_ATTRIBUTES_TEX_COORDS] = 1;
    paperMesh.Initialize(vertexAttributes);
    
    // Create the paper material
    Shader* shader = Renderer::GetInstance()->CreateShader();
    shader->SetSourceFile("Shaders/BurningPaper/vert", "Shaders/BurningPaper/frag");
    Material paperMaterial(shader);

    Texture2D* paperTexture = Renderer::GetInstance()->CreateTexture2DFromFile("Media/paper.jpg");
    Texture2D* noiseTexture = Renderer::GetInstance()->CreateTexture2DFromFile("Media/noise.jpg");
    paperMaterial.SetUniformTexture("paper", paperTexture);
    paperMaterial.SetUniformTexture("noise", noiseTexture);

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

#if PLATFORM == PLATFORM_WIN32
    Text::GetInstance()->SetTextFont("C:/Windows/Fonts/Arial.ttf");
#elif PLATFORM == PLATFORM_LINUX
    Text::GetInstance()->SetTextFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
#endif

    Text::GetInstance()->SetTextSize(24, 24);
    
    float fps = 0;
    float dt = 0.0f;
    size_t numFrames = 0;

    while (window.IsOpen()) {
        begin = clock();

        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

        threshold += t / 3.0f;
        Renderer::GetInstance()->BindShader(shader);
        shader->SetUniformVector2("thresholds", threshold, range);

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->StartRender();
        Renderer::GetInstance()->Render();

        Text::GetInstance()->Write(to_string(fps), 5, 5);

        Renderer::GetInstance()->EndRender();

        Renderer::GetInstance()->PresentFrame();

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
