#include <math/Vector2.h>
#include <math/Vector3.h>

#include <render/Material.h>
#include <render/Mesh.h>
#include <render/Node.h>
#include <render/Renderer.h>
#include <render/SceneTree.h>
#include <render/Shader.h>

#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <string>
#include <vector>
using namespace std;

#include <OIS.h>

#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    Window window("Sample_InteractiveWater", 1024, 768, true);
    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    // Water surface creation
    const int NUM_VERTICES = 16;
    SurfaceTriangles_t surface;
    surface.numVertices = NUM_VERTICES * NUM_VERTICES;
    surface.numTexCoords = NUM_VERTICES * NUM_VERTICES;
    surface.vertices = new Vector3[surface.numVertices];
    surface.texCoords = new Vector2[surface.numTexCoords];

    size_t idx = 0;
    const int NUM_CELLS = NUM_VERTICES - 1;
    float step = 2.0f / NUM_CELLS;
    float uvStep = 1.0f / NUM_VERTICES;

    for (size_t i = 0; i < NUM_VERTICES; i++) {
        for (size_t j = 0; j < NUM_VERTICES; j++) {
            idx = i * NUM_VERTICES + j;
            surface.vertices[idx] = Vector3(j * step - 1.0f, i * step - 1.0f, 1.0f);
            surface.texCoords[idx] = Vector2(uvStep * j, uvStep * i);
        }
    }

    surface.numIndices = NUM_CELLS * NUM_CELLS * 6;
    surface.indices = new unsigned short[surface.numIndices];
    idx = 0;

    for (size_t i = 0; i < NUM_CELLS; i++) {
        for (size_t j = 0; j < NUM_CELLS; j++) {
            surface.indices[idx++] = i * NUM_VERTICES + j;
            surface.indices[idx++] = (i + 1) * NUM_VERTICES + j;
            surface.indices[idx++] = (i + 1) * NUM_VERTICES + j + 1;

            surface.indices[idx++] = i * NUM_VERTICES + j;
            surface.indices[idx++] = (i + 1) * NUM_VERTICES + j + 1;
            surface.indices[idx++] = i * NUM_VERTICES + j + 1;
        }
    }

    ModelSurface_t modelSurface;
    modelSurface.geometry = &surface;

    Mesh waterMesh;
    waterMesh.AddSurface(modelSurface);
    waterMesh.Initialize();

    // Material surface
    vector<string> vertexInputs;
    vertexInputs.push_back("in_vertex");
    vertexInputs.push_back("in_uv");
    Shader* waterShader = Renderer::GetInstance()->CreateShader("Shaders/vert.glsl", "Shaders/frag.glsl", vertexInputs);

    Material waterMaterial(waterShader);

    // Water node
    Node waterNode;
    waterNode.SetMaterial(&waterMaterial);
    waterNode.SetMesh(&waterMesh);
    Renderer::GetInstance()->GetSceneTree().AddChildren(&waterNode);

    Renderer::GetInstance()->CameraLookAt(Vector3(0.0f, 0.0f, -1.5f), Vector3::ZERO);

    // Create the OIS system if present
#ifdef OIS_AVAILABLE

    size_t windowHandle = (size_t)window.GetHandle();

    OIS::ParamList paramList;
    paramList.insert(pair<string, string>("WINDOW", to_string(windowHandle)));

#if defined OIS_WIN32_PLATFORM
    paramList.insert(pair<string, string>("w32_mouse", "DISCL_FOREGROUND"));
    paramList.insert(pair<string, string>("w32_mouse", "DISCL_NONEXCLUSIVE"));
#elif defined OIS_LINUX_PLATFORM
    paramList.insert(pair<string, string>("x11_mouse_grab", "false"));
    paramList.insert(pair<string, string>("x11_mouse_hide", "false"));
#endif

    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(paramList);

    OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, false));
    OIS::Mouse* mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, false));

    const OIS::MouseState& ms = mouse->getMouseState();
    ms.width = window.GetWidth();
    ms.height = window.GetHeight();
#endif

    while (window.IsOpen()) {
        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

        // Poll the mouse if OIS is present
#ifdef OIS_AVAILABLE
        keyboard->capture();

        if (keyboard->isKeyDown(OIS::KC_ESCAPE)) {
            break;
        }

        mouse->capture();

        // If we clicked on the surface, get the point on the water plane and send it to the shader
        // for it to compute the ripples
        if (mouse->getMouseState().buttonDown(OIS::MB_Left)) {
            Vector3 pos = Renderer::GetInstance()->ScreenToWorldPoint(Vector2(mouse->getMouseState().X.abs, mouse->getMouseState().Y.abs));
        }
#endif

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->Render();
        Renderer::GetInstance()->EndRender();
    }

    return 0;
}