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
    const int NUM_VERTICES = 64;
    SurfaceTriangles_t surface;
    surface.numVertices = NUM_VERTICES * NUM_VERTICES;
    surface.numNormals = NUM_VERTICES * NUM_VERTICES;
    surface.numTexCoords = NUM_VERTICES * NUM_VERTICES;
    surface.vertices = new Vector3[surface.numVertices];
    surface.normals = new Vector3[surface.numNormals];
    surface.texCoords = new Vector2[surface.numTexCoords];

    Vector3* buffer = new Vector3[surface.numVertices];
    Vector3* buffer2 = surface.vertices;

    size_t idx = 0;
    const int NUM_CELLS = NUM_VERTICES - 1;
    float step = 2.0f / NUM_CELLS;
    float uvStep = 1.0f / NUM_VERTICES;

    for (size_t i = 0; i < NUM_VERTICES; i++) {
        for (size_t j = 0; j < NUM_VERTICES; j++) {
            idx = i * NUM_VERTICES + j;
            buffer[idx] = surface.vertices[idx] = Vector3(j * step - 1.0f, i * step - 1.0f, 0.0f);
            surface.normals[idx] = -Vector3::LOOK;
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

    Mesh waterMesh(MESH_TYPE_DYNAMIC);
    waterMesh.AddSurface(modelSurface);

    VertexAttributesMap_t vertexAttributes;
    vertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    vertexAttributes[VERTEX_ATTRIBUTES_NORMAL] = 1;
    vertexAttributes[VERTEX_ATTRIBUTES_TEX_COORDS] = 2;
    waterMesh.Initialize(vertexAttributes);

    // Material surface
    Shader* waterShader = Renderer::GetInstance()->CreateShader("Shaders/vert.glsl", "Shaders/frag.glsl");

    Material waterMaterial(waterShader);
    Texture2D* waterTexture = Renderer::GetInstance()->CreateTexture2DFromFile("Media/water.jpg");
    waterMaterial.AddTexture("waterTexture", waterTexture);

    // Water node
    Node waterNode;
    waterNode.SetMaterial(&waterMaterial);
    waterNode.SetMesh(&waterMesh);
    Renderer::GetInstance()->GetSceneTree().AddNode(&waterNode);

    Renderer::GetInstance()->CameraLookAt(Vector3(0.0f, 0.0f, -3.5f), Vector3::ZERO);

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

    //Renderer::GetInstance()->SetRenderFillMode(RENDER_MODE_WIREFRAME);
    float splashForce = 0.1f;
    float damping = 0.999f;
    float maxHeight = 0.15f;

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

            // Transform the position into the range [0, 1] so that we can map it as texture coordinates of the water plane
            // Because the water plane ranges from (-1, -1) to (1, 1), the transformation is trivial
            Vector2 point;
            point.y = pos.x / 2.0f + 0.5f;
            point.x = pos.y / 2.0f + 0.5f;

            if (point.x >= 0.0f && point.y >= 0.0f && point.x <= 1.0f && point.y <= 1.0f) {
                //waterShader->SetUniformVector2("disturbancePoint", pos.x, pos.y);
                size_t i = min(max(point.x * NUM_VERTICES, 1), NUM_VERTICES - 1);
                size_t j = min(max(point.y * NUM_VERTICES, 1), NUM_VERTICES - 1);
                size_t idx = i * NUM_VERTICES + j;

                buffer[idx].z = splashForce;
                buffer[idx + 1].z = splashForce;
                buffer[idx - 1].z = splashForce;
                buffer[(i + 1) * NUM_VERTICES + j].z = splashForce;
                buffer[(i - 1) * NUM_VERTICES + j].z = splashForce;
                buffer[(i + 1) * NUM_VERTICES + j + 1].z = splashForce;
                buffer[(i + 1) * NUM_VERTICES + j - 1].z = splashForce;
                buffer[(i - 1) * NUM_VERTICES + j + 1].z = splashForce;
                buffer[(i - 1) * NUM_VERTICES + j - 1].z = splashForce;

            } else {
                //waterShader->SetUniformVector2("disturbancePoint", -1.0f, -1.0f);
            }
        }
#endif
        for (size_t i = 1; i < NUM_VERTICES - 1; i++) {
            for (size_t j = 1; j < NUM_VERTICES - 1; j++) {
                idx = i * NUM_VERTICES + j;

                buffer[idx].z = (buffer2[idx - 1].z + buffer2[idx + 1].z +
                                 buffer2[(i - 1) * NUM_VERTICES + j].z +
                                 buffer2[(i + 1) * NUM_VERTICES + j].z) / 2.0f - buffer[idx].z;
                buffer[idx].z *= damping;
            }
        }

        for (size_t i = 1; i < NUM_VERTICES - 1; i++) {
            for (size_t j = 1; j < NUM_VERTICES - 1; j++) {
                idx = i * NUM_VERTICES + j;

                surface.normals[idx] = (buffer[idx + 1] - buffer[idx - 1]).Normalized();
            }
        }

        buffer2 = surface.vertices;
        surface.vertices = buffer;
        buffer = buffer2;
        waterMesh.UpdateMeshData();

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->Render();
        Renderer::GetInstance()->EndRender();
    }

    delete[] buffer;

    return 0;
}