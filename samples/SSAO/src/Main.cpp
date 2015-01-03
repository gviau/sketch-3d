#include <render/Material.h>
#include <render/Mesh.h>
#include <render/Node.h>
#include <render/Renderer.h>
#include <render/RenderTexture.h>
#include <render/SceneTree.h>
#include <render/Shader.h>
#include <render/Texture2D.h>

#include <system/Logger.h>
#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <cstdlib>
#include <string>
#include <time.h>
#include <vector>
using namespace std;

#ifdef OIS_AVAILABLE
#include <OIS.h>
#endif

float RandomFloat(float a, float b);

#if defined(__WIN32__) || defined(_WIN32)
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif
    srand(time(nullptr));
    Window window("Sample_SSAO", 1024, 768, true);
    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    // Create the fullscreen quad
    SurfaceTriangles_t surface;
    surface.numVertices = 4;
    surface.numTexCoords = surface.numVertices;
    surface.numIndices = 6;
    surface.vertices = new Vector3[surface.numVertices];
    surface.texCoords = new Vector2[surface.numTexCoords];
    surface.indices = new unsigned short[surface.numIndices];
    surface.vertices[0] = Vector3(-1.0f, -1.0f, 0.0f); surface.vertices[1] = Vector3(-1.0f, 1.0f, 0.0f); surface.vertices[2] = Vector3(1.0f, 1.0f, 0.0f); surface.vertices[3] = Vector3(1.0f, -1.0f, 0.0f);
    surface.texCoords[0] = Vector2(0.0f, 0.0f); surface.texCoords[1] = Vector2(0.0f, 1.0f); surface.texCoords[2] = Vector2(1.0f, 1.0f); surface.texCoords[3] = Vector2(1.0f, 0.0f);
    surface.indices[0] = 0; surface.indices[1] = 3; surface.indices[2] = 2; surface.indices[3] = 0; surface.indices[4] = 2; surface.indices[5] = 1;

    ModelSurface_t surfaceTriangles;
    surfaceTriangles.geometry = &surface;

    Mesh fullscreenQuadMesh;
    fullscreenQuadMesh.AddSurface(surfaceTriangles);
    
    VertexAttributesMap_t fullscreenQuadVertexAttributes;
    fullscreenQuadVertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    fullscreenQuadVertexAttributes[VERTEX_ATTRIBUTES_TEX_COORDS] = 1;
    fullscreenQuadMesh.Initialize(fullscreenQuadVertexAttributes);

    vector<string> fullscreenQuadVertexInputs;
    fullscreenQuadVertexInputs.push_back("in_vertex");
    fullscreenQuadVertexInputs.push_back("in_uv");
    Shader* fullscreenQuadShader = Renderer::GetInstance()->CreateShader("Shaders/SSAO/vert.glsl", "Shaders/SSAO/frag.glsl", fullscreenQuadVertexInputs);

    Material fullscreenQuadMaterial(fullscreenQuadShader);

    Node fullscreenQuad;
    fullscreenQuad.SetMaterial(&fullscreenQuadMaterial);
    fullscreenQuad.SetMesh(&fullscreenQuadMesh);

    // Load the mesh
    VertexAttributesMap_t vertexAttributes;
    vertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    vertexAttributes[VERTEX_ATTRIBUTES_NORMAL] = 1;
    Mesh mesh("Media/pyramob.3ds", vertexAttributes);

    vector<string> vertexInputs;
    vertexInputs.push_back("in_vertex");
    vertexInputs.push_back("in_normal");
    Shader* shader = Renderer::GetInstance()->CreateShader("Shaders/SSAO/record_vert.glsl", "Shaders/SSAO/record_frag.glsl", vertexInputs);

    Material material(shader);

    Node node;
    node.SetMaterial(&material);
    node.SetMesh(&mesh);
    Renderer::GetInstance()->GetSceneTree().AddNode(&node);

    // Create the textures for the render texture
    Texture2D* positionsTexture = Renderer::GetInstance()->CreateTexture2D();
    positionsTexture->SetWidth(1024);
    positionsTexture->SetHeight(768);
    positionsTexture->SetTextureFormat(TEXTURE_FORMAT_RGB32F);
    positionsTexture->SetFilterMode(FILTER_MODE_NEAREST);
    if (!positionsTexture->Create()) {
        Logger::GetInstance()->Error("Couldn't create position texture for render texture");
        return 1;
    }

    Texture2D* normalsTexture = Renderer::GetInstance()->CreateTexture2D();
    normalsTexture->SetWidth(1024);
    normalsTexture->SetHeight(768);
    normalsTexture->SetTextureFormat(TEXTURE_FORMAT_RGB32F);
    normalsTexture->SetFilterMode(FILTER_MODE_NEAREST);
    if (!normalsTexture->Create()) {
        Logger::GetInstance()->Error("Couldn't create normal texture for render texture");
        return 1;
    }

    Texture2D* depthTexture = Renderer::GetInstance()->CreateTexture2D();
    depthTexture->SetWidth(1024);
    depthTexture->SetHeight(768);
    depthTexture->SetTextureFormat(TEXTURE_FORMAT_DEPTH);
    depthTexture->SetFilterMode(FILTER_MODE_NEAREST);
    if (!depthTexture->Create()) {
        Logger::GetInstance()->Error("Couldn't create depth texture for render texture");
        return 1;
    }

    // Create the render texture
    RenderTexture* GBuffer = Renderer::GetInstance()->CreateRenderTexture(1024, 768, TEXTURE_FORMAT_RGB32F);
    vector<Texture2D*> gbufferTextures;
    gbufferTextures.push_back(positionsTexture);
    gbufferTextures.push_back(normalsTexture);
    if (!GBuffer->AttachTextures(gbufferTextures)) {
        Logger::GetInstance()->Error("Couldn't attach textures to the G buffer");
        return 1;
    }

    if (!GBuffer->AttachTextureToDepthBuffer(depthTexture)) {
        Logger::GetInstance()->Error("Couldn't attach a texture to the depth buffer for the G buffer");
        return 1;
    }

    // Create the material for the ssao
    Shader* ssaoShader = Renderer::GetInstance()->CreateShader("Shaders/SSAO/vert.glsl", "Shaders/SSAO/ssao.glsl", fullscreenQuadVertexInputs);
    Material ssaoMaterial(ssaoShader);
    ssaoMaterial.AddTexture("positions", positionsTexture);
    ssaoMaterial.AddTexture("normals", normalsTexture);
    ssaoMaterial.AddTexture("depthTexture", depthTexture);

    //////////////////////////////////////////////////////////////////////////////////
    /// SSAO STUFF - following the implementation presented at http://john-chapman-graphics.blogspot.ca/2013/01/ssao-tutorial.html
    //////////////////////////////////////////////////////////////////////////////////
    // SSAO works like this: we sample in the depth buffer a certain amount of pixels and calculates which one
    // are behind or in front of the geometry. This will give us our attenuation factor. To keep the sample count
    // small, we use a small (4x4) kernel of sample points which we tile. This will result in high frequency samples.
    // We then rotate the kernel to remove the noise generate by this high frequency.

    // Compute the 4x4 kernel
    Vector3 kernel[16];
    for (int i = 0; i < 16; i++) {
        kernel[i] = Vector3(RandomFloat(-1.0f, 1.0f), RandomFloat(-1.0f, 1.0f), RandomFloat(0.0f, 1.0f));
        kernel[i].Normalize();

        float scale = float(i) / 16.0f;
        scale = 0.1f * (1.0f - scale * scale) + 1.0f * scale * scale;
        kernel[i] *= scale;
    }

    // Compute the noise texture
    Texture2D* noise = Renderer::GetInstance()->CreateTexture2D();
    noise->SetWidth(4);
    noise->SetHeight(4);
    noise->SetTextureFormat(TEXTURE_FORMAT_RGB24);
    noise->SetFilterMode(FILTER_MODE_NEAREST);
    if (!noise->Create()) {
        Logger::GetInstance()->Error("Couldn't create noise texture");
        return 1;
    }

    unsigned char noiseData[16 * 3];
    for (int i = 0; i < 16 * 3; i += 3) {
        Vector3 noiseValue(RandomFloat(-1.0f, 1.0f), RandomFloat(-1.0f, 1.0f), 0.0f);
        noiseValue.Normalize();
        noiseData[i    ] = (noiseValue.x / 2.0f + 0.5f) * 255;
        noiseData[i + 1] = (noiseValue.y / 2.0f + 0.5f) * 255;
        noiseData[i + 2] = 0;
    }
    noise->SetPixelDataBytes(noiseData, 4, 4);
    ssaoMaterial.AddTexture("noise", noise);

    // Now we need 1 more render texture for the ssao occlusion factor
    RenderTexture* ssaoOcclusionFactor = Renderer::GetInstance()->CreateRenderTexture(1024, 768, TEXTURE_FORMAT_RGBA32F);
    Texture2D* ssaoTexture = Renderer::GetInstance()->CreateTexture2D();
    ssaoTexture->SetWidth(1024);
    ssaoTexture->SetHeight(768);
    ssaoTexture->SetTextureFormat(TEXTURE_FORMAT_RGBA32F);
    ssaoTexture->SetFilterMode(FILTER_MODE_LINEAR);
    if (!ssaoTexture->Create()) {
        Logger::GetInstance()->Error("Couldn't create ssao texture");
        return 1;
    }

    vector<Texture2D*> ssaoTextures;
    ssaoTextures.push_back(ssaoTexture);
    if (!ssaoOcclusionFactor->AttachTextures(ssaoTextures)) {
        Logger::GetInstance()->Error("Couldn't attach ssao texture to render texture");
        return 1;
    }

    // Attach the G buffer textures and ssao texture to the fullscreen quad material
    fullscreenQuadMaterial.AddTexture("positions", positionsTexture);
    fullscreenQuadMaterial.AddTexture("normals", normalsTexture);
    fullscreenQuadMaterial.AddTexture("ssao", ssaoTexture);

    Renderer::GetInstance()->CameraLookAt(Vector3(0.0f, 0.0f, 150.0f), Vector3::LOOK);

    // Initialize OIS if available
#ifdef OIS_AVAILABLE
    size_t windowHandle = (size_t)window.GetHandle();

    OIS::ParamList paramList;
    paramList.insert(pair<string, string>("WINDOW", to_string(windowHandle)));
    OIS::InputManager* inputManager = OIS::InputManager::createInputSystem(paramList);

    OIS::Keyboard* keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, false));
#endif

    bool doSsao = false;
    float radius = 2.0f;
    while (window.IsOpen()) {
        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

#ifdef OIS_AVAILABLE
        keyboard->capture();

        if (keyboard->isKeyDown(OIS::KC_0)) {
            doSsao = false;
        } else if (keyboard->isKeyDown(OIS::KC_1)) {
            doSsao = true;
        } else if (keyboard->isKeyDown(OIS::KC_UP)) {
            radius += 0.01f;
            if (radius > 3.0f) {
                radius = 3.0f;
            }
        } else if (keyboard->isKeyDown(OIS::KC_DOWN)) {
            radius -= 0.01f;
            if (radius < 0.0f) {
                radius = 0.0f;
            }
        }
#endif

        // Render in GBuffer
        GBuffer->Bind();
        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->Render();

        // Compute ssao occlusion factor
        if (doSsao) {
            ssaoOcclusionFactor->Bind();
            Renderer::GetInstance()->Clear();

            fullscreenQuad.SetMaterial(&ssaoMaterial);
            Renderer::GetInstance()->BindShader(ssaoShader);
            ssaoShader->SetUniformVector2("noiseScale", 256.0f, 192.0f);
            ssaoShader->SetUniformVector3Array("kernel", kernel, 16);
            ssaoShader->SetUniformFloat("radius", radius);
            ssaoShader->SetUniformMatrix4x4("projection", Renderer::GetInstance()->GetProjectionMatrix());

            fullscreenQuad.Render();
        }

        // Draw the result
        Renderer::GetInstance()->BindScreenBuffer();
        Renderer::GetInstance()->Clear();
        fullscreenQuad.SetMaterial(&fullscreenQuadMaterial);
        Renderer::GetInstance()->BindShader(fullscreenQuadShader);
        fullscreenQuadShader->SetUniformInt("doSsao", (doSsao) ? 1 : 0);

        fullscreenQuad.Render();

        Renderer::GetInstance()->EndRender();
    }

    return 0;
}

float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}