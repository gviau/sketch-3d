#include <math/Matrix4x4.h>
#include <math/Quaternion.h>
#include <math/Vector2.h>
#include <math/Vector3.h>
#include <math/Vector4.h>

#include <render/Material.h>
#include <render/Mesh.h>
#include <render/Node.h>
#include <render/Renderer.h>
#include <render/RenderTexture.h>
#include <render/SceneTree.h>
#include <render/Shader.h>
#include <render/Texture2D.h>
#include <render/Texture3D.h>

#include <system/Logger.h>
#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <vector>
#include <string>
#include <time.h>
using namespace std;

#ifdef OIS_AVAILABLE
#include <OIS.h>
#endif

#define NUM_LIGHTS 4

void UpdateLights(double t, Vector3 initialLightPositions[], Vector3 newLightPositions[]);

#if PLATFORM == PLATFORM_WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif
    Window window("Sample_Shadow", 1024, 768, true);
    RenderParameters_t renderParameters;
    renderParameters.width = 1024;
    renderParameters.height = 768;
    renderParameters.displayFormat = DISPLAY_FORMAT_X8R8G8B8;
    renderParameters.refreshRate = 0;
    renderParameters.depthStencilBits = DEPTH_STENCIL_BITS_D24X8;

    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window, renderParameters);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    // Create a render texture for the shadow map
    RenderTexture* shadowMaps[4];
    Texture2D* shadowMapTextures[4];

    for (size_t i = 0; i < NUM_LIGHTS; i++) {
        shadowMaps[i] = Renderer::GetInstance()->CreateRenderTexture(1024, 1024, TEXTURE_FORMAT_DEPTH);
        shadowMapTextures[i] = shadowMaps[i]->CreateDepthBufferTexture();
        shadowMapTextures[i]->SetFilterMode(FILTER_MODE_LINEAR);

        if (!shadowMaps[i]->AttachTextureToDepthBuffer(shadowMapTextures[i])) {
            Logger::GetInstance()->Error("Couldn't create shadow map");
        }
    }

    // Create the shader
    Shader* shader = Renderer::GetInstance()->CreateShader();
    shader->SetSourceFile("Shaders/Shadow/vert", "Shaders/Shadow/frag");
    Material material(shader);

    // Create the jeep
    VertexAttributesMap_t vertexAttributes;
    vertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    vertexAttributes[VERTEX_ATTRIBUTES_NORMAL] = 1;
    vertexAttributes[VERTEX_ATTRIBUTES_TEX_COORDS] = 2;
    Mesh jeepMesh("Media/jeep1.3ds", vertexAttributes);
    Node jeepNode1;
    jeepNode1.SetMaterial(&material);
    jeepNode1.SetMesh(&jeepMesh);

    // The jeep mesh is not oriented correctly for our needs
    Quaternion orientation;
    orientation.MakeFromAngleAxis(-PI_OVER_2, Vector3::RIGHT);

    jeepNode1.SetOrientation(orientation);
    Renderer::GetInstance()->GetSceneTree().AddNode(&jeepNode1);

    // Create a second jeep
    Node jeepNode2;
    jeepNode2.SetMaterial(&material);
    jeepNode2.SetMesh(&jeepMesh);
    jeepNode2.SetOrientation(orientation);
    jeepNode2.SetPosition(Vector3(-15.0f, 0.0f, -12.0f));
    Renderer::GetInstance()->GetSceneTree().AddNode(&jeepNode2);

    // Create the record shadow material
    Shader* recordShadowShader = Renderer::GetInstance()->CreateShader();
    recordShadowShader->SetSourceFile("Shaders/Shadow/recordShadow_vert", "Shaders/Shadow/recordShadow_frag");
    Material recordShadowMaterial(recordShadowShader);

    // Create the ground
    Mesh groundMesh;

    SurfaceTriangles_t surface;
    surface.numVertices = 4;
    surface.numNormals = 4;
    surface.numTexCoords = 4;
    surface.numIndices = 6;
    surface.numTextures = 1;
    surface.vertices = new Vector3[surface.numVertices];
    surface.normals = new Vector3[surface.numNormals];
    surface.texCoords = new Vector2[surface.numTexCoords];
    surface.indices = new unsigned short[surface.numIndices];
    surface.textures = new Texture2D* [surface.numTextures];
    
    surface.vertices[0] = Vector3(-50.0f, 0.0f, -50.0f); surface.vertices[1] = Vector3(-50.0f, 0.0f, 50.0f); surface.vertices[2] = Vector3(50.0f, 0.0f, 50.0f); surface.vertices[3] = Vector3(50.0f, 0.0f, -50.0f);
    surface.normals[0] = surface.normals[1] = surface.normals[2] = surface.normals[3] = Vector3::UP;
    surface.texCoords[0] = Vector2(0.0f, 0.0f); surface.texCoords[1] = Vector2(0.0f, 1.0f); surface.texCoords[2] = Vector2(1.0f, 1.0f); surface.texCoords[3] = Vector2(1.0f, 0.0f);
    surface.indices[0] = 0; surface.indices[1] = 1; surface.indices[2] = 2; surface.indices[3] = 0; surface.indices[4] = 2; surface.indices[5] = 3;
    surface.textures[0] = Renderer::GetInstance()->CreateTexture2DFromFile("Media/tiled.jpg");

    groundMesh.AddSurface(&surface);
    groundMesh.Initialize(vertexAttributes);

    // Create the ground
    Node groundNode;
    groundNode.SetMaterial(&material);
    groundNode.SetMesh(&groundMesh);
    Renderer::GetInstance()->GetSceneTree().AddNode(&groundNode);

    // Define some useful vectors for this sample
    Vector3 cameraPosition(-20.0f, 15.0f, -55.0f);

    // Defines the lights that we have
    Vector3 lightPositions[4];
    Vector4 lightColors[4];
    Vector3 newLightPositions[4];

    newLightPositions[0] = lightPositions[0] = Vector3(15.0f, 20.0f, 15.0f);
    newLightPositions[1] = lightPositions[1] = Vector3(-40.0f, 15.0f, -30.0f);
    newLightPositions[2] = lightPositions[2] = Vector3(15.0f, 30.0f, -20.0f);
    newLightPositions[3] = lightPositions[3] = Vector3(-20.0f, 15.0f, 20.0f);
    lightColors[0] = Vector4(1.0f, 1.0f, 1.0f, 100.0f);
    lightColors[1] = Vector4(0.8f, 0.0f, 0.0f, 100.0f);
    lightColors[2] = Vector4(0.0f, 0.8f, 0.0f, 100.0f);
    lightColors[3] = Vector4(0.0f, 0.0f, 0.8f, 100.0f);

    Renderer::GetInstance()->SetBlendingFactor(BLENDING_FACTOR_ONE, BLENDING_FACTOR_ONE);

    double t = 0.0;
    clock_t begin;
    clock_t end;

    // Initialize OIS if available
#ifdef OIS_AVAILABLE
    size_t windowHandle = (size_t)window.GetHandle();

    OIS::ParamList paramList;
    paramList.insert(pair<string, string>("WINDOW", to_string(windowHandle)));
    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(paramList);

    OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, false));
#endif

    size_t currentLightNumber = 1;

    while (window.IsOpen()) {
        begin = clock();

        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

        // Movement & change the number of lights
#ifdef OIS_AVAILABLE
        keyboard->capture();

        if (keyboard->isKeyDown(OIS::KC_1)) {
            currentLightNumber = 1;
        } else if (keyboard->isKeyDown(OIS::KC_2)) {
            currentLightNumber = 2;
        } else if (keyboard->isKeyDown(OIS::KC_3)) {
            currentLightNumber = 3;
        } else if (keyboard->isKeyDown(OIS::KC_4)) {
            currentLightNumber = 4;
        }
#endif

        Renderer::GetInstance()->EnableDepthWrite(true);
        Renderer::GetInstance()->SetDepthComparisonFunc(DEPTH_FUNC_LESS);

        // Update the lights' positions
        UpdateLights(t, lightPositions, newLightPositions);

        // First, we render from each light's perspective and record the depth in their respective shadow map
        Renderer::GetInstance()->SetCullingMethod(CULLING_METHOD_FRONT_FACE);
        jeepNode1.SetMaterial(&recordShadowMaterial);
        jeepNode2.SetMaterial(&recordShadowMaterial);

        Renderer::GetInstance()->StartRender();

        for (size_t i = 0; i < currentLightNumber; i++) {
            shadowMaps[i]->Bind();
            Renderer::GetInstance()->PerspectiveProjection(65.0f, 1024.0f/768.0f, 1.0f, 1000.0f);
            Renderer::GetInstance()->CameraLookAt(newLightPositions[i], Vector3::ZERO, Vector3::UP);
            Renderer::GetInstance()->Clear();
            
            // Only render shadow casters
            jeepNode1.Render();
            jeepNode2.Render();
        }

        // Then we use the shadow maps to render the scene normally from the camera's perspective
        Renderer::GetInstance()->BindScreenBuffer();
        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->SetCullingMethod(CULLING_METHOD_BACK_FACE);
        jeepNode1.SetMaterial(&material);
        jeepNode2.SetMaterial(&material);
        Renderer::GetInstance()->BindShader(shader);

        for (size_t i = 0; i < currentLightNumber; i++) {
            // If there's more than one light, we activate additive blending to accumulate each light contribution in
            // the framebuffer. Moreover, we disable depth writing and set the depth comparison function to less or equal
            // to prevent Z-fighting.
            if (i == 1) {
                Renderer::GetInstance()->EnableDepthWrite(false);
                Renderer::GetInstance()->SetDepthComparisonFunc(DEPTH_FUNC_EQUAL);
                Renderer::GetInstance()->EnableBlending(true);
            }

            Renderer::GetInstance()->PerspectiveProjection(65.0f, 1024.0f/768.0f, 1.0f, 1000.0f);
            Renderer::GetInstance()->CameraLookAt(newLightPositions[i], Vector3::ZERO, Vector3::UP);
            Matrix4x4 shadowMatrix = Renderer::GetInstance()->GetViewProjectionMatrix();
            shader->SetUniformMatrix4x4("shadowMatrix", shadowMatrix);
            shader->SetUniformVector3("light_position", newLightPositions[i]);
            shader->SetUniformVector4("light_color", lightColors[i]);
            shader->SetUniformVector2("shadowMapTexelStep", 1.0f / shadowMapTextures[i]->GetWidth(), 1.0f / shadowMapTextures[i]->GetHeight());
            material.SetUniformTexture("shadowMap", shadowMapTextures[i]);

            Renderer::GetInstance()->PerspectiveProjection(45.0f, 1024.0f/768.0f, 1.0f, 1000.0f);
            Renderer::GetInstance()->CameraLookAt(cameraPosition, Vector3::ZERO, Vector3::UP);
            Renderer::GetInstance()->Render();
        }

        Renderer::GetInstance()->EndRender();

        Renderer::GetInstance()->PresentFrame();

        if (currentLightNumber > 1) {
            Renderer::GetInstance()->EnableBlending(false);
        }

        end = clock();
        t += double(end - begin) / CLOCKS_PER_SEC;
    }

    return 0;
}

void UpdateLights(double t, Vector3 initialLightPositions[], Vector3 newLightPositions[]) {
    for (size_t i = 0; i < NUM_LIGHTS; i++) {
        float direction = (i % 2 == 0) ? 1.0f : -1.0f;
        newLightPositions[i].x = 75.0f * float(cos(direction * t / (i + 1)));
        newLightPositions[i].z = 75.0f * float(sin(direction * t / (i + 1)));
    }
}
