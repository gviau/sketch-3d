#include <render/Material.h>
#include <render/Mesh.h>
#include <render/Node.h>
#include <render/Renderer.h>
#include <render/SceneTree.h>
#include <render/Shader.h>
#include <render/Text.h>

#include <system/Logger.h>
#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <chrono>
using namespace std;

#ifdef OIS_AVAILABLE
#include <OIS.h>
#endif

#define NUM_TEAPOTS_X 100
#define NUM_TEAPOTS_Z 100
#define NUM_TEAPOTS   NUM_TEAPOTS_X * NUM_TEAPOTS_Z

#if PLATFORM == PLATFORM_WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif

    /*
    ConfigFileAttributes_t configFileAttributes;;
    if (!ParseConfigFile("init.cfg", configFileAttributes)) {
        Logger::GetInstance()->Error("Error while reading config file");
        return 1;
    }
    
    RenderParameters_t renderParameters;
    renderParameters.width = configFileAttributes.width;
    renderParameters.height = configFileAttributes.height;
    renderParameters.displayFormat = configFileAttributes.displayFormat;
    renderParameters.depthStencilBits = configFileAttributes.depthStencilBits;
    renderParameters.refreshRate = configFileAttributes.refreshRate;
    */

    RenderParameters_t renderParameters;
    renderParameters.width = 1024;
    renderParameters.height = 768;
    renderParameters.displayFormat = DISPLAY_FORMAT_X8R8G8B8;
    renderParameters.depthStencilBits = DEPTH_STENCIL_BITS_D24X8;
    renderParameters.refreshRate = 0;
    Window window("Sample_FrustumCullingInstancing", 1024, 768, true);
    Renderer::GetInstance()->Initialize(RenderSystem_t::OPENGL, window, renderParameters);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    VertexAttributesMap_t vertexAttributes;
    vertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    vertexAttributes[VERTEX_ATTRIBUTES_NORMAL] = 1;
    Mesh mesh("Media/teapot.nff", vertexAttributes, MESH_TYPE_STATIC, false);

    Shader* shader = Renderer::GetInstance()->CreateShader();
    shader->SetSourceFile("Shaders/FrustumCullingInstancing/vert", "Shaders/FrustumCullingInstancing/frag");
    Material material(shader);

    Node* nodes = new Node[NUM_TEAPOTS];
    float stepX = 1.0f;
    float stepZ = 1.0f;
    size_t idx = 0;
    for (size_t i = 0; i < NUM_TEAPOTS_X; i++) {
        for (size_t j = 0; j < NUM_TEAPOTS_Z; j++) {
            nodes[idx].SetMaterial(&material);
            nodes[idx].SetMesh(&mesh);
            Renderer::GetInstance()->GetSceneTree().AddNode(&nodes[idx]);
            nodes[idx].Pitch(-PI_OVER_2);
            nodes[idx].Scale(Vector3(0.2f, 0.2f, 0.2f));
            nodes[idx].SetPosition(Vector3(-NUM_TEAPOTS_X / 2.0f + stepX * j, 0.0f, -NUM_TEAPOTS_Z / 2.0f + stepZ * i));

            nodes[idx].SetInstancing(true);
            idx += 1;
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    // Initialize OIS if available
    ////////////////////////////////////////////////////////////////////////////
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

    const OIS::MouseState& msSetup = mouse->getMouseState();
    msSetup.width = window.GetWidth();
    msSetup.height = window.GetHeight();
#endif

    ////////////////////////////////////////////////////////////////////////////
    // Information regarding camera
    ////////////////////////////////////////////////////////////////////////////
    Vector3 right = Vector3::RIGHT;
    Vector3 up = Vector3::UP;
    Vector3 look = Vector3::LOOK;

    float xAngle = 0.0f;
    float yAngle = 0.0f;
    float moveSpeed = 5.0f;
    float mouseSpeed = 4.0f;

#if PLATFORM == PLATFORM_WIN32
    Text::GetInstance()->SetTextFont("C:/Windows/Fonts/Arial.ttf");
#elif PLATFORM == PLATFORM_LINUX
    Text::GetInstance()->SetTextFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
#endif

    Text::GetInstance()->SetTextSize(24, 24);

    float fps = 0;
    float t = 0.0f;
    size_t numFrames = 0;
    double dt = 0.0;
    clock_t begin;
    clock_t end;
    bool frustumCulling = true;

    Renderer::GetInstance()->CameraLookAt(Vector3(0.0f, 3.0f, 0.0f), Vector3(0.0f, 3.0f, -1.0f));

    while (window.IsOpen()) {
        begin = clock();

        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

#ifdef OIS_AVAILABLE
        keyboard->capture();
        if (keyboard->isKeyDown(OIS::KC_ESCAPE)) {
            break;
        }

        if (keyboard->isKeyDown(OIS::KC_R)) {
            right = Vector3::RIGHT;
            up = Vector3::UP;
            look = Vector3::LOOK;
        }

        static bool frustumDebouncer = false;
        if (!frustumDebouncer && keyboard->isKeyDown(OIS::KC_F)) {
            frustumCulling = !frustumCulling;
            Renderer::GetInstance()->EnableFrustumCulling(frustumCulling);

            frustumDebouncer = true;
        } else if (frustumDebouncer && !keyboard->isKeyDown(OIS::KC_F)) {
            frustumDebouncer = false;
        }

        mouse->capture();
        const OIS::MouseState& mouseState = mouse->getMouseState();
        int x = mouseState.X.rel;
        int y = mouseState.Y.rel;

        float angleY = 0.0f;
        if (x != 0) {
            if (x > 0) {
                angleY = mouseSpeed * (float)dt;
            } else if (x < 0) {
                angleY = -mouseSpeed * (float)dt;
            }

            Matrix3x3 yaw;
            yaw.RotateAroundAxis(Vector3::UP, angleY);
            right *= yaw;
            look *= yaw;
        }

        float angleX = 0.0f;
        if (y != 0) {
            if (y > 0) {
                angleX = mouseSpeed * (float)dt;
            } else if (y < 0) {
                angleX = -mouseSpeed * (float)dt;
            }
            
            Matrix3x3 pitch;
            pitch.RotateAroundAxis(right, angleX);
            up *= pitch;
            look *= pitch;
        }
#endif

        // Set the camera view matrix
        look.Normalize();

        up = look.Cross(right);
        up.Normalize();

        right = up.Cross(look);
        right.Normalize();

        Renderer::GetInstance()->SetViewMatrix(right, up, look, Vector3(0.0f, 3.0f, 0.0f));

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->StartRender();
        Renderer::GetInstance()->Render();

        Text::GetInstance()->Write("FPS: " + to_string(fps), 5, 5);

        Renderer::GetInstance()->EndRender();

        Renderer::GetInstance()->PresentFrame();

        end = clock();
        dt = double(end - begin) / CLOCKS_PER_SEC;
        t += float(end - begin) / CLOCKS_PER_SEC;

        numFrames += 1;
        if (t >= 0.25f) {
            fps = (float)numFrames / t;
            numFrames = 0;
            t -= 0.25f;
        }
    }

    delete[] nodes;

    return 0;
}