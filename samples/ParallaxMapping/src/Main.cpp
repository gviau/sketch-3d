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

#if PLATFORM == PLATFORM_WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif
    Window window("Sample_ParallaxMapping", 1024, 768, true);
    RenderParameters_t renderParameters;
    renderParameters.width = 1024;
    renderParameters.height = 768;
    renderParameters.displayFormat = DISPLAY_FORMAT_X8R8G8B8;
    renderParameters.refreshRate = 0;
    renderParameters.depthStencilBits = DEPTH_STENCIL_BITS_D24X8;

    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window, renderParameters);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    // Load the sphere's mesh and generate the tangents
    //Mesh sphereMesh("Media/sphere.obj", true, true, true);
    SurfaceTriangles_t surface;
    surface.numVertices = 4;
    surface.numNormals = 4;
    surface.numTexCoords = 4;
    surface.numTangents = 4;
    surface.vertices = new Vector3[surface.numVertices];
    surface.normals = new Vector3[surface.numNormals];
    surface.texCoords = new Vector2[surface.numTexCoords];
    surface.tangents = new Vector3[surface.numTangents];
    surface.vertices[0] = Vector3(-1.0f, -1.0f, 1.0f); surface.vertices[1] = Vector3(-1.0f, 1.0f, 1.0f); surface.vertices[2] = Vector3(1.0f, 1.0f, 1.0f); surface.vertices[3] = Vector3(1.0f, -1.0f, 1.0f);
    surface.normals[0] = surface.normals[1] = surface.normals[2] = surface.normals[3] = -Vector3::LOOK;
    surface.texCoords[0] = Vector2(0.0f, 0.0f); surface.texCoords[1] = Vector2(0.0f, 1.0f); surface.texCoords[2] = Vector2(1.0f, 1.0f); surface.texCoords[3] = Vector2(1.0f, 0.0f);
    surface.tangents[0] = surface.tangents[1] = surface.tangents[2] = surface.tangents[3] = Vector3::RIGHT;
    
    surface.numIndices = 6;
    surface.indices = new unsigned short[surface.numIndices];
    surface.indices[0] = 0; surface.indices[1] = 1; surface.indices[2] = 2; surface.indices[3] = 0; surface.indices[4] = 2; surface.indices[5] = 3;

    Mesh mesh;
    mesh.AddSurface(&surface);

    VertexAttributesMap_t vertexAttributes;
    vertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    vertexAttributes[VERTEX_ATTRIBUTES_NORMAL] = 1;
    vertexAttributes[VERTEX_ATTRIBUTES_TEX_COORDS] = 2;
    vertexAttributes[VERTEX_ATTRIBUTES_TANGENT] = 3;
    mesh.Initialize(vertexAttributes);

    // Create the material for the sphere
    Shader* shader = Renderer::GetInstance()->CreateShader();
    shader->SetSourceFile("Shaders/ParallaxMapping/vert", "Shaders/ParallaxMapping/frag");
    Material sphereMaterial(shader);

    // Add the diffuse and normal texture
    Texture2D* diffuseTexture = Renderer::GetInstance()->CreateTexture2DFromFile("Media/rock_wall.jpg");
    Texture2D* normalTexture = Renderer::GetInstance()->CreateTexture2DFromFile("Media/rock_wall_normal.jpg");
    Texture2D* heightTexture = Renderer::GetInstance()->CreateTexture2DFromFile("Media/rock_wall_height.jpg");
    sphereMaterial.SetUniformTexture("diffuseTexture", diffuseTexture);
    sphereMaterial.SetUniformTexture("normalTexture", normalTexture);
    sphereMaterial.SetUniformTexture("heightTexture", heightTexture);

    // Create the sphere node
    Node sphereNode;
    sphereNode.SetMaterial(&sphereMaterial);
    sphereNode.SetMesh(&mesh);
    Renderer::GetInstance()->GetSceneTree().AddNode(&sphereNode);

    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = -1.0f;
    float speedX = 1.0f;
    float speedY = 1.0f;
    float speedZ = 1.0f;

    // Create the OIS system if present
#ifdef OIS_AVAILABLE

    size_t windowHandle = (size_t)window.GetHandle();

    OIS::ParamList paramList;
    paramList.insert(pair<string, string>("WINDOW", to_string(windowHandle)));
    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(paramList);

    OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, false));
#endif

    float delta = 0.0f;
    clock_t begin, end;

    while (window.IsOpen()) {
        begin = clock();

        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

        // Handle input if OIS is present
#ifdef OIS_AVAILABLE
        keyboard->capture();

        if (keyboard->isKeyDown(OIS::KC_ESCAPE)) {
            break;
        }

        if (keyboard->isKeyDown(OIS::KC_W)) {
            posZ += delta * speedZ;
        } else if (keyboard->isKeyDown(OIS::KC_S)) {
            posZ -= delta * speedZ;
        }

        if (keyboard->isKeyDown(OIS::KC_A)) {
            posX -= delta * speedX;
        } else if (keyboard->isKeyDown(OIS::KC_D)) {
            posX += delta * speedX;
        }

        if (keyboard->isKeyDown(OIS::KC_Q)) {
            posY += delta * speedY;
        } else if (keyboard->isKeyDown(OIS::KC_E)) {
            posY -= delta * speedY;
        }
#endif

        Renderer::GetInstance()->CameraLookAt(Vector3(posX, posY, posZ), Vector3::LOOK);

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->StartRender();
        Renderer::GetInstance()->Render();
        Renderer::GetInstance()->EndRender();

        Renderer::GetInstance()->PresentFrame();

        end = clock();
        delta = float(end - begin) / (float)CLOCKS_PER_SEC;
    }

    return 0;
}
