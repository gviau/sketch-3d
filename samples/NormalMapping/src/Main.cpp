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

#ifdef OIS_AVAILABLE
#include <OIS.h>
#endif

#if defined(__WIN32__) || defined(_WIN32)
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif
    Window window("Sample_NormalMapping", 1024, 768, true);
    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    Mesh dummy;
    dummy.ConstructBoundingSphere();

    // Load the sphere's mesh and generate the tangents
    VertexAttributesMap_t vertexAttributes;
    vertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    vertexAttributes[VERTEX_ATTRIBUTES_NORMAL] = 1;
    vertexAttributes[VERTEX_ATTRIBUTES_TEX_COORDS] = 2;
    vertexAttributes[VERTEX_ATTRIBUTES_TANGENT] = 3;
    Mesh sphereMesh("Media/sphere.obj", vertexAttributes);

    // Create the material for the sphere
    Shader* shader = Renderer::GetInstance()->CreateShader("Shaders/NormalMapping/vert", "Shaders/NormalMapping/frag");
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
    Renderer::GetInstance()->GetSceneTree().AddNode(&sphereNode);

    Renderer::GetInstance()->CameraLookAt(Vector3(0.0f, 0.0f, -3.0f), Vector3::ZERO);

    // Create the OIS system if present
#ifdef OIS_AVAILABLE

    size_t windowHandle = (size_t)window.GetHandle();

    OIS::ParamList paramList;
    paramList.insert(pair<string, string>("WINDOW", to_string(windowHandle)));

    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(paramList);

    OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, false));
#endif

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
            sphereNode.Yaw(t);
        } else if (keyboard->isKeyDown(OIS::KC_D) || keyboard->isKeyDown(OIS::KC_RIGHT)) {
            sphereNode.Yaw(-t);
        }

        if (keyboard->isKeyDown(OIS::KC_W) || keyboard->isKeyDown(OIS::KC_UP)) {
            sphereNode.Pitch(t);
        } else if (keyboard->isKeyDown(OIS::KC_S) || keyboard->isKeyDown(OIS::KC_DOWN)) {
            sphereNode.Pitch(-t);
        }
#endif

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->StartRender();
        Renderer::GetInstance()->Render();
        Renderer::GetInstance()->EndRender();

        Renderer::GetInstance()->PresentFrame();

        end = clock();
        t = float(end - begin) / (float)CLOCKS_PER_SEC;
    }

    return 0;
}
