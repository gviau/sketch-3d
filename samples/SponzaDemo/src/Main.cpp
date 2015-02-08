#include <math/Matrix4x4.h>
#include <math/Vector4.h>

#include <render/Material.h>
#include <render/Mesh.h>
#include <render/Node.h>
#include <render/Renderer.h>
#include <render/RenderTexture.h>
#include <render/SceneTree.h>
#include <render/Shader.h>
#include <render/SkinnedMesh.h>
#include <render/Texture2D.h>

#include <system/Logger.h>
#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <chrono>
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
    Window window("Sample_SponzaDemo", 1024, 768, true);
    RenderParameters_t renderParameters;
    renderParameters.width = 1024;
    renderParameters.height = 768;
    renderParameters.displayFormat = DISPLAY_FORMAT_X8R8G8B8;
    renderParameters.refreshRate = 0;
    renderParameters.depthStencilBits = DEPTH_STENCIL_BITS_D24X8;

    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window, renderParameters);
    Renderer::GetInstance()->SetClearColor(0.1f, 0.1f, 0.1f);
    Renderer::GetInstance()->PerspectiveProjection(45.0f, 1024.0f/768.0f, 1.0f, 500.0f);
    Renderer::GetInstance()->EnableFrustumCulling(false);

    ////////////////////////////////////////////////////////////////////////////
    // Create the GBuffer render texture
    ////////////////////////////////////////////////////////////////////////////
    RenderTexture* GBuffer = Renderer::GetInstance()->CreateRenderTexture(window.GetWidth(), window.GetHeight(), TEXTURE_FORMAT_RGBA32F);
    Texture2D* normalsTexture = GBuffer->CreateTexture2D();
    Texture2D* albedosTexture = GBuffer->CreateTexture2D();
    Texture2D* depthTexture = GBuffer->CreateDepthBufferTexture();
   
    vector<Texture2D*> gbufferTextures;
    gbufferTextures.push_back(normalsTexture);
    gbufferTextures.push_back(albedosTexture);
    if (!GBuffer->AttachTextures(gbufferTextures)) {
        Logger::GetInstance()->Error("Couldn't attach textures to GBuffer");
        return 1;
    }

    if (!GBuffer->AttachTextureToDepthBuffer(depthTexture)) {
        Logger::GetInstance()->Error("Couldn't attach depth texture to GBuffer");
        return 1;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Load the sponza model
    ////////////////////////////////////////////////////////////////////////////
    VertexAttributesMap_t sponzaVertexAttributes;
    sponzaVertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    sponzaVertexAttributes[VERTEX_ATTRIBUTES_NORMAL] = 1;
    sponzaVertexAttributes[VERTEX_ATTRIBUTES_TEX_COORDS] = 2;
    Mesh sponzaMesh("Media/sponza.obj", sponzaVertexAttributes, MESH_TYPE_STATIC, false);

    // Create the material for the sponza
    Shader* sponzaShader = Renderer::GetInstance()->CreateShader("Shaders/SponzaDemo/sponza_vert", "Shaders/SponzaDemo/sponza_frag");
    Material sponzaMaterial(sponzaShader);

    // Create the node for the ponza
    Node sponza;
    sponza.SetMaterial(&sponzaMaterial);
    sponza.SetMesh(&sponzaMesh);
    Renderer::GetInstance()->GetSceneTree().AddNode(&sponza);

    ////////////////////////////////////////////////////////////////////////////
    // Load the hellknight model
    ////////////////////////////////////////////////////////////////////////////
    VertexAttributesMap_t hellknightVertexAttributes;
    hellknightVertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    hellknightVertexAttributes[VERTEX_ATTRIBUTES_NORMAL] = 1;
    hellknightVertexAttributes[VERTEX_ATTRIBUTES_TEX_COORDS] = 2;
    hellknightVertexAttributes[VERTEX_ATTRIBUTES_TANGENT] = 3;
    hellknightVertexAttributes[VERTEX_ATTRIBUTES_BONES] = 4;
    hellknightVertexAttributes[VERTEX_ATTRIBUTES_WEIGHTS] = 5;
    SkinnedMesh hellknightMesh("Media/models/md5/monsters/hellknight/hellknight.md5mesh", hellknightVertexAttributes, true, false);

    // Create the material for the hellknight
    Shader* hellknightShader = Renderer::GetInstance()->CreateShader("Shaders/SponzaDemo/hellknight_vert", "Shaders/SponzaDemo/hellknight_frag");
    Material hellknightMaterial(hellknightShader);

    // Create the node for the hellknight
    Node hellknight;
    hellknight.SetMaterial(&hellknightMaterial);
    hellknight.SetMesh(&hellknightMesh);

    hellknight.Yaw(2.75f);
    hellknight.SetPosition(Vector3(0.0f, 0.05f, 0.0f));
    hellknight.Scale(Vector3(0.025f, 0.025f, 0.025f));
    Renderer::GetInstance()->GetSceneTree().AddNode(&hellknight);

    hellknightMesh.SetAnimationState("Default");
    hellknightMesh.SetAnimationLoop(true);

    ////////////////////////////////////////////////////////////////////////////
    // Create the fullscreen quad mesh
    ////////////////////////////////////////////////////////////////////////////
    SurfaceTriangles_t surface;
    surface.numVertices = 4;
    surface.numIndices = 6;
    surface.vertices = new Vector3[surface.numVertices];
    surface.indices = new unsigned short[surface.numIndices];
    surface.vertices[0] = Vector3(-1.0f, -1.0f, 0.0f); surface.vertices[1] = Vector3(-1.0f, 1.0f, 0.0f); surface.vertices[2] = Vector3(1.0f, 1.0f, 0.0f); surface.vertices[3] = Vector3(1.0f, -1.0f, 0.0f);
    surface.indices[0] = 0; surface.indices[1] = 3; surface.indices[2] = 2; surface.indices[3] = 0; surface.indices[4] = 2; surface.indices[5] = 1;

    // Precompute the ray at each corner used to reconstruct the position from the depth values. Store them in
    // normals to pass them to shader
    Matrix4x4 inverseProjection = Renderer::GetInstance()->GetProjectionMatrix().Inverse();
    Vector4 homogenousCorners[4] = {
        Vector4(-1.0f, -1.0f, -1.0f, 1.0f),
        Vector4(-1.0f,  1.0f, -1.0f, 1.0f),
        Vector4( 1.0f,  1.0f, -1.0f, 1.0f),
        Vector4( 1.0f, -1.0f, -1.0f, 1.0f)
    };

    surface.numNormals = surface.numVertices;
    surface.normals = new Vector3[surface.numNormals];

    for (int i = 0; i < 4; i++) {
        homogenousCorners[i] *= inverseProjection;
        homogenousCorners[i] /= homogenousCorners[i].w;

        // Z-normalize
        homogenousCorners[i] /= homogenousCorners[i].z;
        surface.normals[i] = Vector3(homogenousCorners[i].x, homogenousCorners[i].y, homogenousCorners[i].z);
    }

    Mesh fullscreenQuadMesh;
    fullscreenQuadMesh.AddSurface(&surface);

    VertexAttributesMap_t fullscreenQuadVertexAttributes;
    fullscreenQuadVertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    fullscreenQuadVertexAttributes[VERTEX_ATTRIBUTES_NORMAL] = 1;
    fullscreenQuadMesh.Initialize(fullscreenQuadVertexAttributes);

    // Create the fullscreen quad material
    Shader* fullscreenQuadShader = Renderer::GetInstance()->CreateShader("Shaders/SponzaDemo/fullscreen_vert", "Shaders/SponzaDemo/fullscreen_frag");
    Material fullscreenQuadMaterial(fullscreenQuadShader);

    fullscreenQuadMaterial.AddTexture("normals", normalsTexture);
    fullscreenQuadMaterial.AddTexture("albedos", albedosTexture);
    fullscreenQuadMaterial.AddTexture("depth", depthTexture);

    // Create the fullscreen quad node
    Node fullscreenQuad;
    fullscreenQuad.SetMaterial(&fullscreenQuadMaterial);
    fullscreenQuad.SetMesh(&fullscreenQuadMesh);

    ////////////////////////////////////////////////////////////////////////////
    // Shadow maps creation
    ////////////////////////////////////////////////////////////////////////////
    RenderTexture* shadowMap0 = Renderer::GetInstance()->CreateRenderTexture(2048, 2048, TEXTURE_FORMAT_DEPTH);
    Texture2D* shadowMapTexture0 = shadowMap0->CreateDepthBufferTexture();

    if (!shadowMap0->AttachTextureToDepthBuffer(shadowMapTexture0)) {
        Logger::GetInstance()->Error("Couldn't attach shadow map texture to depth buffer for shadow map #0");
        return 1;
    }

    fullscreenQuadMaterial.AddTexture("shadowMap0", shadowMapTexture0);

    // Create the shader used to record the shadows
    Shader* sponzaRecordShadowShader = Renderer::GetInstance()->CreateShader("Shaders/SponzaDemo/sponza_record_shadow_vert",
                                                                             "Shaders/SponzaDemo/record_shadow_frag");
    Material sponzaRecordShadowMaterial(sponzaRecordShadowShader);

    Shader* hellknightRecordShadowShader = Renderer::GetInstance()->CreateShader("Shaders/SponzaDemo/hellknight_record_shadow_vert",
                                                                                 "Shaders/SponzaDemo/record_shadow_frag");
    Material hellknightRecordShadowMaterial(hellknightRecordShadowShader);

    ////////////////////////////////////////////////////////////////////////////
    // Information regarding camera
    ////////////////////////////////////////////////////////////////////////////
    Vector3 cameraPosition(0.0f, 5.0f, 0.0f);
    Vector3 right = Vector3::RIGHT;
    Vector3 up = Vector3::UP;
    Vector3 look = Vector3::LOOK;

    float xAngle = 0.0f;
    float yAngle = 0.0f;
    float moveSpeed = 5.0f;
    float mouseSpeed = 4.0f;

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
    // Light and misc stuff
    ////////////////////////////////////////////////////////////////////////////
    Matrix4x4 projectionMatrix = Renderer::GetInstance()->GetProjectionMatrix();
    Vector2 positionReconstructionParams(projectionMatrix[2][3], projectionMatrix[2][2]);

    Vector3 ambientLight(0.1f, 0.1f, 0.1f);
    Vector3 lightPosition0(-16.523f, 2.5f, 0.0f);
    Vector3 lightColor0(1.0f, 1.0f, 1.0f);
    Vector3 lightLook0(0.998115f, -0.055971f, 0.025161f);

    Vector3 lightPosition1(8.5997f, 7.935f, 5.6665f);
    Vector3 lightColor1(1.0f, 1.0f, 0.2f);
    Vector3 lightLook1(-0.826325f, -0.123682f, 0.549445f);

    Vector3 lightPosition2(-7.2518f, 13.5501f, 0.053049f);
    Vector3 lightColor2(0.2f, 1.0f, 1.0f);
    Vector3 lightLook2(0.765129f, -0.290197f, 0.579713f);

    Matrix4x4 biasMatrix(0.5f, 0.0f, 0.0f, 0.5f,
                         0.0f, 0.5f, 0.0f, 0.5f,
                         0.0f, 0.0f, 0.5f, 0.5f,
                         0.0f, 0.0f, 0.0f, 1.0f);

    double dt = 0.0;
    clock_t begin;
    clock_t end;

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

        if (keyboard->isKeyDown(OIS::KC_W) || keyboard->isKeyDown(OIS::KC_UP)) {
            cameraPosition += Vector3(look.x, 0.0f, look.z) * moveSpeed * (float)dt;
        } else if (keyboard->isKeyDown(OIS::KC_S) || keyboard->isKeyDown(OIS::KC_DOWN)) {
            cameraPosition -= Vector3(look.x, 0.0f, look.z)  * moveSpeed * (float)dt;
        }

        if (keyboard->isKeyDown(OIS::KC_A) || keyboard->isKeyDown(OIS::KC_LEFT)) {
            cameraPosition -= Vector3(right.x, 0.0f, right.z)  * moveSpeed * (float)dt;
        } else if (keyboard->isKeyDown(OIS::KC_D) || keyboard->isKeyDown(OIS::KC_RIGHT)) {
            cameraPosition += Vector3(right.x, 0.0f, right.z)  * moveSpeed * (float)dt;
        }

        if (keyboard->isKeyDown(OIS::KC_Q)) {
            cameraPosition.y += moveSpeed * (float)dt;
        } else if (keyboard->isKeyDown(OIS::KC_E)) {
            cameraPosition.y -= moveSpeed * (float)dt;
        }

        static bool debouncer = false;
        static bool lockMouse = false;

        if (!debouncer && keyboard->isKeyDown(OIS::KC_P)) {
            Logger::GetInstance()->Warning("Position => x: " + to_string(cameraPosition.x) + " y: " + to_string(cameraPosition.y) + " z: " + to_string(cameraPosition.z));
            Logger::GetInstance()->Warning("Look => x: " + to_string(look.x) + " y: " + to_string(look.y) + " z: " + to_string(look.z));
            debouncer = true;
        } else if (debouncer && !keyboard->isKeyDown(OIS::KC_P)) {
            debouncer = false;
        }

        if (!debouncer && keyboard->isKeyDown(OIS::KC_SPACE)) {
            lockMouse = !lockMouse;
            debouncer = true;
        } else if (debouncer && !keyboard->isKeyDown(OIS::KC_SPACE)) {
            debouncer = false;
        }

        if (!lockMouse) {
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
        }
#endif
        // Draw into the shadow maps
        shadowMap0->Bind();

        vector<Matrix4x4> boneTransformationMatrices;
        hellknightMesh.Animate(dt, boneTransformationMatrices);

        sponza.SetMaterial(&sponzaRecordShadowMaterial);
        hellknight.SetMaterial(&hellknightRecordShadowMaterial);

        Renderer::GetInstance()->BindShader(hellknightRecordShadowShader);
        hellknightRecordShadowShader->SetUniformMatrix4x4Array("boneTransformationMatrices", &boneTransformationMatrices[0], boneTransformationMatrices.size());

        Renderer::GetInstance()->SetCullingMethod(CULLING_METHOD_FRONT_FACE);
        Renderer::GetInstance()->PerspectiveProjection(90.0f, 1024.0f/768.0f, 1.0f, 500.0f);

        // Render from the light's perspective
        Renderer::GetInstance()->CameraLookAt(lightPosition0, lightPosition0 + lightLook0);

        // Prepare the shadow matrix right now
        Matrix4x4 shadowMatrix0 = biasMatrix * Renderer::GetInstance()->GetViewProjectionMatrix();

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->Render();

        // Draw into the GBuffer
        GBuffer->Bind();

        sponza.SetMaterial(&sponzaMaterial);
        hellknight.SetMaterial(&hellknightMaterial);
        Renderer::GetInstance()->BindShader(hellknightShader);

        Renderer::GetInstance()->SetCullingMethod(CULLING_METHOD_BACK_FACE);
        Renderer::GetInstance()->PerspectiveProjection(45.0f, 1024.0f/768.0f, 1.0f, 500.0f);

        // Animate the hellknight
        hellknightShader->SetUniformMatrix4x4Array("boneTransformationMatrices", &boneTransformationMatrices[0], boneTransformationMatrices.size());

        // Set the camera view matrix
        look.Normalize();

        up = look.Cross(right);
        up.Normalize();

        right = up.Cross(look);
        right.Normalize();

        Renderer::GetInstance()->SetViewMatrix(right, up, look, cameraPosition);

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->Render();

        // Draw into the fullscreen quad
        Renderer::GetInstance()->BindScreenBuffer();
        Renderer::GetInstance()->Clear();

        Renderer::GetInstance()->BindShader(fullscreenQuadShader);
        fullscreenQuadShader->SetUniformVector2("positionReconstructionParams", positionReconstructionParams.x, positionReconstructionParams.y);
        fullscreenQuadShader->SetUniformVector3("ambientLight", Vector3(0.1f, 0.1f, 0.1f));
        fullscreenQuadShader->SetUniformVector3("lightPosition0", lightPosition0);
        fullscreenQuadShader->SetUniformVector3("lightColor0", lightColor0);
        fullscreenQuadShader->SetUniformMatrix4x4("shadowMatrix0", shadowMatrix0);
        fullscreenQuadShader->SetUniformVector3("lightPosition1", lightPosition1);
        fullscreenQuadShader->SetUniformVector3("lightColor1", lightColor1);
        fullscreenQuadShader->SetUniformVector3("lightPosition2", lightPosition2);
        fullscreenQuadShader->SetUniformVector3("lightColor2", lightColor1);

        fullscreenQuad.Render();

        Renderer::GetInstance()->EndRender();
        Renderer::GetInstance()->PresentFrame();

        end = clock();
        dt = double(end - begin) / CLOCKS_PER_SEC;
    }

    return 0;
}