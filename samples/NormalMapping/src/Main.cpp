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
#include <time.h>
#include <vector>
using namespace std;

#include <Windows.h>

#ifdef OIS_AVAILABLE
#include <OIS.h>
#endif

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

    // Create the OIS system if present
#ifdef OIS_AVAILABLE

    size_t windowHandle = (size_t)window.GetHandle();

    OIS::ParamList paramList;
    paramList.insert(pair<string, string>("WINDOW", to_string(windowHandle)));

    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(paramList);

    OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, false));
#endif

    float angleY = 0.0f;
    float angleX = 0.0f;
    float t = 0.0f;
    clock_t begin, end;

    while (window.IsOpen()) {
        begin = clock();

        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

        // Rotate the sphere
#ifdef OIS_AVAILABLE
        keyboard->capture();

        if (keyboard->isKeyDown(OIS::KC_ESCAPE)) {
            break;
        }

        if (keyboard->isKeyDown(OIS::KC_A) || keyboard->isKeyDown(OIS::KC_LEFT)) {
            angleY += t;
        } else if (keyboard->isKeyDown(OIS::KC_D) || keyboard->isKeyDown(OIS::KC_RIGHT)) {
            angleY -= t;
        }

        if (keyboard->isKeyDown(OIS::KC_W) || keyboard->isKeyDown(OIS::KC_UP)) {
            angleX += t;
        } else if (keyboard->isKeyDown(OIS::KC_S) || keyboard->isKeyDown(OIS::KC_DOWN)) {
            angleX -= t;
        }

        sphereNode.Yaw(angleY);
        //sphereNode.Pitch(angleX);
#endif

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->Render();
        Renderer::GetInstance()->EndRender();

        end = clock();
        t = float(end - begin) / (float)CLOCKS_PER_SEC;
    }

    return 0;
}