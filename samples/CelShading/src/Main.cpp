#include <math/Quaternion.h>
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

#ifdef OIS_AVAILABLE
#include <OIS.h>
#endif

#if defined(__WIN32__) || defined(_WIN32)
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif

    Window window("Sample_CelShading", 1024, 768, true);
    RenderParameters_t renderParameters;
    renderParameters.width = 1024;
    renderParameters.height = 768;
    renderParameters.displayFormat = DISPLAY_FORMAT_X8R8G8B8;
    renderParameters.refreshRate = 0;
    renderParameters.depthStencilBits = DEPTH_STENCIL_BITS_D24X8;

    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window, renderParameters);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    // Load the mesh
    VertexAttributesMap_t vertexAttributes;
    vertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    vertexAttributes[VERTEX_ATTRIBUTES_NORMAL] = 1;
    Mesh teapotMesh("Media/teapot.nff", vertexAttributes);

    // Create the material
    Shader* normalRenderShader = Renderer::GetInstance()->CreateShader("Shaders/CelShading/normalRender_vert", "Shaders/CelShading/normalRender_frag");
    Shader* contourRenderShader = Renderer::GetInstance()->CreateShader("Shaders/CelShading/contourRender_vert", "Shaders/CelShading/contourRender_frag");

    Material normalRenderMaterial(normalRenderShader);
    Material contourRenderMaterial(contourRenderShader);

    // Create the node
    Node teapotNode;
    teapotNode.SetMesh(&teapotMesh);
    Renderer::GetInstance()->GetSceneTree().AddNode(&teapotNode);

    Renderer::GetInstance()->CameraLookAt(Vector3(0.0f, 2.5f, -7.5f), Vector3::LOOK);
    Renderer::GetInstance()->SetBlendingEquation(BLENDING_EQUATION_ADD);
    Renderer::GetInstance()->SetBlendingFactor(BLENDING_FACTOR_ONE, BLENDING_FACTOR_ONE);

    // Initialize OIS if available
#ifdef OIS_AVAILABLE
    size_t windowHandle = (size_t)window.GetHandle();

    OIS::ParamList paramList;
    paramList.insert(pair<string, string>("WINDOW", to_string(windowHandle)));

    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(paramList);

    OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, false));
#endif

    float teapotAngle = 0.0f;

    while (window.IsOpen()) {
        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

#ifdef OIS_AVAILABLE
        keyboard->capture();

        if (keyboard->isKeyDown(OIS::KC_ESCAPE)) {
            break;
        }
#endif
        Quaternion rotationX, rotationY;
        rotationX.MakeFromAngleAxis(-PI_OVER_2, Vector3::RIGHT);
        rotationY.MakeFromAngleAxis(teapotAngle, Vector3::LOOK);
        teapotNode.SetOrientation(rotationX * rotationY);
        teapotAngle += 0.0005f;

        Renderer::GetInstance()->Clear();

        Renderer::GetInstance()->StartRender();
        
        Renderer::GetInstance()->SetCullingMethod(CULLING_METHOD_FRONT_FACE);
        teapotNode.SetMaterial(&contourRenderMaterial);
        Renderer::GetInstance()->BindShader(contourRenderShader);
        contourRenderShader->SetUniformFloat("offset", 0.015f);
        Renderer::GetInstance()->Render();
        
        Renderer::GetInstance()->SetCullingMethod(CULLING_METHOD_BACK_FACE);
        teapotNode.SetMaterial(&normalRenderMaterial);
        Renderer::GetInstance()->Render();
        
        Renderer::GetInstance()->EndRender();

        Renderer::GetInstance()->PresentFrame();
    }

    return 0;
}