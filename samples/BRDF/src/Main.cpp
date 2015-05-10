#include <math/Vector3.h>

#include <render/Material.h>
#include <render/Mesh.h>
#include <render/Node.h>
#include <render/Renderer.h>
#include <render/RenderTexture.h>
#include <render/SceneTree.h>
#include <render/Shader.h>
#include <render/Text.h>
#include <render/Texture2D.h>

#include <system/Logger.h>
#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <time.h>

#ifdef OIS_AVAILABLE
#include <OIS.h>
#endif

#if PLATFORM == PLATFORM_WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif
    Window window("Sample_BRDF", 1024, 768, true);
    RenderParameters_t renderParameters;
    renderParameters.width = 1024;
    renderParameters.height = 768;
    renderParameters.displayFormat = DISPLAY_FORMAT_X8R8G8B8;
    renderParameters.depthStencilBits = DEPTH_STENCIL_BITS_D24X8;
    renderParameters.refreshRate = 0;
    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window, renderParameters);
    Renderer::GetInstance()->SetClearColor(0.0f, 0.0f, 0.0f);

    // Load the meshes
    VertexAttributesMap_t sphereVertexAttributes;
    sphereVertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    sphereVertexAttributes[VERTEX_ATTRIBUTES_NORMAL] = 1;
    sphereVertexAttributes[VERTEX_ATTRIBUTES_TEX_COORDS] = 2;
    sphereVertexAttributes[VERTEX_ATTRIBUTES_TANGENT] = 3;
    Mesh sphereMesh("Media/sphere.obj", sphereVertexAttributes);

    VertexAttributesMap_t teapotVertexAttributes;
    teapotVertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    teapotVertexAttributes[VERTEX_ATTRIBUTES_NORMAL] = 1;
    Mesh teapotMesh("Media/teapot.nff", teapotVertexAttributes);

    // Load the different materials
    Shader* shader = Renderer::GetInstance()->CreateShader();
    Shader* normalMappedShader = Renderer::GetInstance()->CreateShader();
    shader->SetSourceFile("Shaders/BRDF/vert", "Shaders/BRDF/frag");
    normalMappedShader->SetSourceFile("Shaders/BRDF/normalMapped_vert", "Shaders/BRDF/normalMapped_frag");

    Material material(shader);
    Material normalMappedMaterial(normalMappedShader);

    // Add the diffuse and normal texture for the sphere
    Texture2D* diffuseTexture = Renderer::GetInstance()->CreateTexture2DFromFile("Media/rock_wall.jpg");
    Texture2D* normalTexture = Renderer::GetInstance()->CreateTexture2DFromFile("Media/rock_wall_normal.jpg");
    normalMappedMaterial.AddTexture("diffuseTexture", diffuseTexture);
    normalMappedMaterial.AddTexture("normalTexture", normalTexture);

    // Create the actual nodes
    Node sphereNode;
    sphereNode.SetMaterial(&material);
    sphereNode.SetMesh(&sphereMesh);

    Node normalMappedSphereNode;
    normalMappedSphereNode.SetMaterial(&normalMappedMaterial);
    normalMappedSphereNode.SetMesh(&sphereMesh);

    Node teapotNode;
    teapotNode.SetMaterial(&material);
    teapotNode.SetMesh(&teapotMesh);
    teapotNode.SetScale(Vector3(0.3f, 0.3f, 0.3f));
    Quaternion orientation;
    orientation.MakeFromAngleAxis(-PI_OVER_2, Vector3::RIGHT);
    teapotNode.SetOrientation(orientation);

    // Create a floating-point texture for HDR rendering when using PBR BRDF
    RenderTexture* hdrRenderTexture = Renderer::GetInstance()->CreateRenderTexture(1024, 768, TEXTURE_FORMAT_RGBA16F);
    Texture2D* hdrTexture = hdrRenderTexture->CreateTexture2D();
    vector<Texture2D*> textures;
    textures.push_back(hdrTexture);
    if (!hdrRenderTexture->AttachTextures(textures)) {
        Logger::GetInstance()->Error("Couldn't attach FP16 texture to render target");
        return 1;
    }
    hdrRenderTexture->AddDepthBuffer();

    // Create a fullscreen quad
    Mesh fullscreenQuadMesh;
    SurfaceTriangles_t surface;
    surface.numVertices = 4;
    surface.numIndices = 6;
    surface.vertices = new Vector3[surface.numVertices];
    surface.indices = new unsigned short[surface.numIndices];
    surface.vertices[0] = Vector3(-1.0f, -1.0f, 0.0f); surface.vertices[1] = Vector3(-1.0f, 1.0f, 0.0f); surface.vertices[2] = Vector3(1.0f, 1.0f, 0.0f); surface.vertices[3] = Vector3(1.0f, -1.0f, 0.0f);
    surface.indices[0] = 0; surface.indices[1] = 2; surface.indices[2] = 1; surface.indices[3] = 0; surface.indices[4] = 3; surface.indices[5] = 2;
    fullscreenQuadMesh.AddSurface(&surface);

    VertexAttributesMap_t fullscreenQuadVertexAttributes;
    fullscreenQuadVertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    fullscreenQuadMesh.Initialize(fullscreenQuadVertexAttributes);

    Shader* fullscreenQuadShader = Renderer::GetInstance()->CreateShader();
    fullscreenQuadShader->SetSourceFile("Shaders/BRDF/fullscreenQuad_vert", "Shaders/BRDF/fullscreenQuad_frag");

    Material fullscreenQuadMaterial(fullscreenQuadShader);
    fullscreenQuadMaterial.AddTexture("hdrTexture", hdrTexture);

    Node fullscreenQuadNode;
    fullscreenQuadNode.SetMaterial(&fullscreenQuadMaterial);
    fullscreenQuadNode.SetMesh(&fullscreenQuadMesh);

    Renderer::GetInstance()->CameraLookAt(Vector3(0.0f, 0.0f, 3.0f), Vector3::ZERO);

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

    // Variables used to cycle through meshes and materials
    size_t currentNodeIndex = 0;
    Node* nodes[] = { &sphereNode, &normalMappedSphereNode, &teapotNode };
    size_t num_nodes = sizeof(nodes) / sizeof(Node*);

    bool rotateObject = true;
    float angleY = 0.0f;
    int brdfIndex = 0;

    vector<string> brdfNames;
    brdfNames.push_back("Textbook Blinn-Phong");
    brdfNames.push_back("Blinn-Phong");
    brdfNames.push_back("Cook-Torrance");

    // Debouncers for keyboard
    bool mesh_debouncer = false;
    bool rotate_debouncer = false;

    // Misc variables
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

    // For camera orbit
    float r = 3.0f;
    float theta = 0.0f;
    float phi = 0.0f;

    // Material parameters
    float specularPower = 32.0f;
    float roughness = 1.0f;
    float R0 = 0.05f;

    while (window.IsOpen()) {
        begin = clock();

#ifdef OIS_AVAILABLE
        keyboard->capture();
        if (keyboard->isKeyDown(OIS::KC_ESCAPE)) {
            break;
        }

        if (keyboard->isKeyDown(OIS::KC_Q)) {
            specularPower = min(specularPower + 1.0f, 1024.0f);
        } else if (keyboard->isKeyDown(OIS::KC_A)) {
            specularPower = max(specularPower - 1.0f, 1.0f);
        }

        if (brdfIndex == 2) {
            if (keyboard->isKeyDown(OIS::KC_W)) {
                roughness = min(roughness + 0.005f, 1.0f);
            } else if (keyboard->isKeyDown(OIS::KC_S)) {
                roughness = max(roughness - 0.005f, 0.0f);
            }

            if (keyboard->isKeyDown(OIS::KC_E)) {
                R0 = min(R0 + 0.005f, 1.0f);
            } else if (keyboard->isKeyDown(OIS::KC_D)) {
                R0 = max(R0 - 0.005f, 0.0f);
            }
        }

        if (keyboard->isKeyDown(OIS::KC_SPACE) && !mesh_debouncer) {
            currentNodeIndex = (currentNodeIndex + 1) % num_nodes;
            mesh_debouncer = true;
        } else if (!keyboard->isKeyDown(OIS::KC_SPACE) && mesh_debouncer) {
            mesh_debouncer = false;
        }

        if (keyboard->isKeyDown(OIS::KC_Z) && !rotate_debouncer) {
            rotateObject = !rotateObject;
            rotate_debouncer = true;
        } else if (!keyboard->isKeyDown(OIS::KC_Z) && rotate_debouncer) {
            rotate_debouncer = false;
        }

        if (keyboard->isKeyDown(OIS::KC_1)) {
            brdfIndex = 0;
        } else if (keyboard->isKeyDown(OIS::KC_2)) {
            brdfIndex = 1;
        } else if (keyboard->isKeyDown(OIS::KC_3)) {
            brdfIndex = 2;
        }

        mouse->capture();
        const OIS::MouseState& mouseState = mouse->getMouseState();
        int x = mouseState.X.rel;
        int y = mouseState.Y.rel;
        int z = mouseState.Z.rel;

        if (mouseState.buttonDown(OIS::MB_Left)) {
            if (x > 0) {
                theta += PI_OVER_2 * t;
            } else if (x < 0) {
                theta -= PI_OVER_2 * t;
            }

            if (y < 0) {
                phi += PI_OVER_2 * t;
            } else if (y > 0) {
                phi -= PI_OVER_2 * t;
            }
        }

        if (z > 0) {
            r = max(r - 0.05f, 1.0f);
        } else if (z < 0) {
            r = min(r + 0.05f, 10.0f);
        }

        Renderer::GetInstance()->CameraLookAt(Vector3(r * sinf(theta) * cosf(phi), r * sinf(theta) * sinf(phi), r * cosf(theta)), Vector3::ZERO);
#endif

        if (brdfIndex == 2) {
            hdrRenderTexture->Bind();
        } else {
            Renderer::GetInstance()->BindScreenBuffer();
        }

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->StartRender();
        
        Shader* currentShader = nodes[currentNodeIndex]->GetMaterial()->GetShader();
        Renderer::GetInstance()->BindShader(currentShader);
        currentShader->SetUniformVector3("light_position", Vector3(10.0f, 1.5f, 0.75f));
        currentShader->SetUniformFloat("specular_power", specularPower);
        currentShader->SetUniformFloat("roughness", roughness);
        currentShader->SetUniformFloat("R0", R0);
        currentShader->SetUniformInt("brdfIndex", brdfIndex);

        if (rotateObject) {
            nodes[currentNodeIndex]->Yaw(PI * t * 0.05f);
        }

        nodes[currentNodeIndex]->Render();

        // If we're rendering PBR BRDFs, we have to tonemap the output
        if (brdfIndex == 2) {
            Renderer::GetInstance()->EndRender();

            Renderer::GetInstance()->BindScreenBuffer();
            Renderer::GetInstance()->Clear();
            Renderer::GetInstance()->StartRender();

            fullscreenQuadNode.Render();

            Renderer::GetInstance()->EnableDepthTest(false);
        }

        Text::GetInstance()->Write(to_string(fps), 5, 5);
        Text::GetInstance()->Write(brdfNames[brdfIndex], 5, 25);

        if (brdfIndex == 2) {
            Text::GetInstance()->Write("Roughness (m): " + to_string(roughness), 5, 45);
            Text::GetInstance()->Write("Fresnel reference (R0): " + to_string(R0), 5, 65);

            Renderer::GetInstance()->EnableDepthTest(true);
        } else {
            Text::GetInstance()->Write("Specular power: " + to_string(specularPower), 5, 45);
            Text::GetInstance()->Write("x: " + to_string(x), 5, 65);
            Text::GetInstance()->Write("y: " + to_string(y), 6, 85);
        }

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
