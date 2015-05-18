#include <math/Matrix4x4.h>
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

#include <fstream>
#include <time.h>
using namespace std;

#if PLATFORM == PLATFORM_WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif
    Window window("Sample_Bokeh", 1024, 768, true);

    RenderParameters_t renderParameters;
    renderParameters.width = 1024;
    renderParameters.height = 768;
    renderParameters.displayFormat = DISPLAY_FORMAT_X8R8G8B8;
    renderParameters.depthStencilBits = DEPTH_STENCIL_BITS_D24X8;
    renderParameters.refreshRate = 0;
    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window, renderParameters);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    //////////////////////////////////////////////////////////////////////////
    // Create the nodes of the scene
    //////////////////////////////////////////////////////////////////////////
    VertexAttributesMap_t vertexAttributes;
    vertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    vertexAttributes[VERTEX_ATTRIBUTES_NORMAL] = 1;
    vertexAttributes[VERTEX_ATTRIBUTES_TEX_COORDS] = 2;
    vertexAttributes[VERTEX_ATTRIBUTES_TANGENT] = 3;
    Mesh tankMesh("Media/TankScene/tank_geoShape.obj", vertexAttributes);
    Mesh groundMesh("Media/TankScene/ground_planeShape.obj", vertexAttributes);
    Mesh barricadeMesh("Media/TankScene/barricade_geoShape.obj", vertexAttributes);

    Shader* shader = Renderer::GetInstance()->CreateShader();
    shader->SetSourceFile("Shaders/Bokeh/vert", "Shaders/Bokeh/frag");
    Material tankMaterial(shader);
    Material groundMaterial(shader);
    Material barricadeMaterial(shader);

    Shader* recordDepthShader = Renderer::GetInstance()->CreateShader();
    recordDepthShader->SetSourceFile("Shaders/Bokeh/recordDepth_vert", "Shaders/Bokeh/recordDepth_frag");
    Material recordDepthMaterial(recordDepthShader);

    Node tankNode;
    tankNode.SetMaterial(&tankMaterial);
    tankNode.SetMesh(&tankMesh);
    Renderer::GetInstance()->GetSceneTree().AddNode(&tankNode);

    Node groundNode;
    groundNode.SetMaterial(&groundMaterial);
    groundNode.SetMesh(&groundMesh);
    Renderer::GetInstance()->GetSceneTree().AddNode(&groundNode);
    
    Node barricadeNode;
    barricadeNode.SetMaterial(&barricadeMaterial);
    barricadeNode.SetMesh(&barricadeMesh);
    Renderer::GetInstance()->GetSceneTree().AddNode(&barricadeNode);

    //////////////////////////////////////////////////////////////////////////
    // Create a fullscreen quad
    //////////////////////////////////////////////////////////////////////////
    VertexAttributesMap_t fullscreenQuadVertexAttributes;
    fullscreenQuadVertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    Mesh fullscreenQuadMesh;

    SurfaceTriangles_t surface;
    surface.numVertices = 4;
    surface.numIndices = 6;
    surface.vertices = new Vector3[surface.numVertices];
    surface.indices = new unsigned short[surface.numIndices];
    surface.vertices[0] = Vector3(-1.0f, -1.0f, 0.0f); surface.vertices[1] = Vector3(-1.0f, 1.0f, 0.0f); surface.vertices[2] = Vector3(1.0f, 1.0f, 0.0f); surface.vertices[3] = Vector3(1.0f, -1.0f, 0.0f);
    surface.indices[0] = 0; surface.indices[1] = 3; surface.indices[2] = 2; surface.indices[3] = 0; surface.indices[4] = 2; surface.indices[5] = 1;

    fullscreenQuadMesh.AddSurface(&surface);
    fullscreenQuadMesh.Initialize(fullscreenQuadVertexAttributes);

    Node fullscreenQuadNode;
    fullscreenQuadNode.SetMesh(&fullscreenQuadMesh);

    //////////////////////////////////////////////////////////////////////////
    // Create a skybox
    //////////////////////////////////////////////////////////////////////////
    Shader* skyboxShader = Renderer::GetInstance()->CreateShader();
    skyboxShader->SetSourceFile("Shaders/Bokeh/skybox_vert", "Shaders/Bokeh/skybox_frag");
    Material skyboxMaterial(skyboxShader);

    Node skyboxNode;
    skyboxNode.SetMesh(&fullscreenQuadMesh);
    skyboxNode.SetMaterial(&skyboxMaterial);

    Shader* debugShader = Renderer::GetInstance()->CreateShader();
    debugShader->SetSourceFile("Shaders/Bokeh/fullscreenQuad", "Shaders/Bokeh/debug");
    Material debugMaterial(debugShader);

    Node debugQuad;
    debugQuad.SetMesh(&fullscreenQuadMesh);
    debugQuad.SetMaterial(&debugMaterial);

    //////////////////////////////////////////////////////////////////////////
    // Create the render textures for special effects like shadow mapping
    //////////////////////////////////////////////////////////////////////////
    RenderTexture* renderTexture = Renderer::GetInstance()->CreateRenderTexture(2048, 2048, TEXTURE_FORMAT_DEPTH);
    Texture2D* shadowMap = renderTexture->CreateDepthBufferTexture();
    shadowMap->SetFilterMode(FILTER_MODE_LINEAR);
    if (!renderTexture->AttachTextureToDepthBuffer(shadowMap)) {
        Logger::GetInstance()->Error("Couldn't attach texture to depth buffer");
        return 1;
    }
    tankMaterial.SetUniformTexture("shadowMap", shadowMap);
    groundMaterial.SetUniformTexture("shadowMap", shadowMap);
    barricadeMaterial.SetUniformTexture("shadowMap", shadowMap);

    //////////////////////////////////////////////////////////////////////////
    // Fullscale HDR sample
    //////////////////////////////////////////////////////////////////////////
    RenderTexture* hdrRenderTexture = Renderer::GetInstance()->CreateRenderTexture(1024, 768, TEXTURE_FORMAT_RGBA16F);
    Texture2D* hdrTexture = hdrRenderTexture->CreateTexture2D();
    vector<Texture2D*> textures;
    textures.push_back(hdrTexture);
    if (!hdrRenderTexture->AttachTextures(textures)) {
        Logger::GetInstance()->Error("Couldn't attach texture to hdr render texture");
        return 1;
    }

    if (!hdrRenderTexture->AddDepthBuffer()) {
        Logger::GetInstance()->Error("Couldn't add depth buffer to hdr render texture");
        return 1;
    }

    //////////////////////////////////////////////////////////////////////////
    // 1/4 scale HDR sample (for blurring)
    //////////////////////////////////////////////////////////////////////////
    RenderTexture* downSampleRenderTexture = Renderer::GetInstance()->CreateRenderTexture(512, 384, TEXTURE_FORMAT_RGBA16F);
    Texture2D* downSample = downSampleRenderTexture->CreateTexture2D();
    downSample->SetFilterMode(FILTER_MODE_LINEAR);
    textures[0] = downSample;
    if (!downSampleRenderTexture->AttachTextures(textures)) {
        Logger::GetInstance()->Error("Couldn't attach down sample texture to render texture");
        return 1;
    }

    //////////////////////////////////////////////////////////////////////////
    // Result of horizontal blur
    //////////////////////////////////////////////////////////////////////////
    RenderTexture* horizontalBlurRenderTexture = Renderer::GetInstance()->CreateRenderTexture(512, 384, TEXTURE_FORMAT_RGBA16F);
    Texture2D* horizontalBlur = horizontalBlurRenderTexture->CreateTexture2D();
    horizontalBlur->SetFilterMode(FILTER_MODE_LINEAR);
    textures[0] = horizontalBlur;
    if (!horizontalBlurRenderTexture->AttachTextures(textures)) {
        Logger::GetInstance()->Error("Couldn't attach horizontal blur texture to render texture");
        return 1;
    }

    //////////////////////////////////////////////////////////////////////////
    // Result of vertical blur
    //////////////////////////////////////////////////////////////////////////
    RenderTexture* verticalBlurRenderTexture = Renderer::GetInstance()->CreateRenderTexture(512, 384, TEXTURE_FORMAT_RGBA16F);
    Texture2D* verticalBlur = horizontalBlurRenderTexture->CreateTexture2D();
    verticalBlur->SetFilterMode(FILTER_MODE_LINEAR);
    textures[0] = verticalBlur;
    if (!verticalBlurRenderTexture->AttachTextures(textures)) {
        Logger::GetInstance()->Error("Couldn't attach vertical blur texture to render texture");
        return 1;
    }

    //////////////////////////////////////////////////////////////////////////
    // Shaders for post processing effects
    //////////////////////////////////////////////////////////////////////////
    Shader* downSampleShader = Renderer::GetInstance()->CreateShader();
    downSampleShader->SetSourceFile("Shaders/Bokeh/quarterQuad", "Shaders/Bokeh/HDR/downSample");
    Material downSampleMaterial(downSampleShader);
    downSampleMaterial.SetUniformTexture("fullSample", hdrTexture);

    Shader* blurXShader = Renderer::GetInstance()->CreateShader();
    blurXShader->SetSourceFile("Shaders/Bokeh/quarterQuad", "Shaders/Bokeh/HDR/blurX");
    Material blurXMaterial(blurXShader);
    blurXMaterial.SetUniformTexture("sampleToBlur", downSample);

    Shader* blurYShader = Renderer::GetInstance()->CreateShader();
    blurYShader->SetSourceFile("Shaders/Bokeh/quarterQuad", "Shaders/Bokeh/HDR/blurY");
    Material blurYMaterial(blurYShader);
    blurYMaterial.SetUniformTexture("sampleToBlur", horizontalBlur);

    Shader* toneMappingShader = Renderer::GetInstance()->CreateShader();
    toneMappingShader->SetSourceFile("Shaders/Bokeh/fullscreenQuad", "Shaders/Bokeh/HDR/toneMapping");
    Material toneMappingMaterial(toneMappingShader);
    toneMappingMaterial.SetUniformTexture("HDRTexture", hdrTexture);
    toneMappingMaterial.SetUniformTexture("bluredHDR", verticalBlur);

    //////////////////////////////////////////////////////////////////////////
    // Load the baked H-basis coefficients for the 3 models
    //////////////////////////////////////////////////////////////////////////
    // 19 x 1163
    ifstream bakedCoef("Media/TankScene/TankScene_tank.mbd", ios::binary);
    if (!bakedCoef.is_open()) {
        Logger::GetInstance()->Error("Couldn't open file TankScene_tank.mbd");
        return 1;
    }
    
    bakedCoef.seekg(0, ios::end);
    size_t coefSize = (size_t)bakedCoef.tellg();
    coefSize -= 4;
    bakedCoef.seekg(4, ios::beg);
    vector<unsigned char> tankBakedCoefficients;
    tankBakedCoefficients.resize(coefSize);
    bakedCoef.read((char*)&tankBakedCoefficients[0], coefSize);

    bakedCoef.close();

    // 16 x 257
    bakedCoef.open("Media/TankScene/TankScene_ground.mbd", ios::binary);
    if (!bakedCoef.is_open()) {
        Logger::GetInstance()->Error("Couldn't open file TankScene_ground.mbd");
        return 1;
    }

    bakedCoef.seekg(0, ios::end);
    coefSize = (size_t)bakedCoef.tellg();
    coefSize -= 4;
    bakedCoef.seekg(4, ios::beg);
    vector<unsigned char> groundBakedCoefficients;
    groundBakedCoefficients.resize(coefSize);
    bakedCoef.read((char*)&groundBakedCoefficients[0], coefSize);

    bakedCoef.close();

    // 32 x 305
    bakedCoef.open("Media/TankScene/TankScene_barricade.mbd", ios::binary);
    if (!bakedCoef.is_open()) {
        Logger::GetInstance()->Error("Couldn't open file TankScene_barricade.mbd");
        return 1;
    }

    bakedCoef.seekg(0, ios::end);
    coefSize = (size_t)bakedCoef.tellg();
    coefSize -= 4;
    bakedCoef.seekg(4, ios::beg);
    vector<unsigned char> barricadeBakedCoefficients;
    barricadeBakedCoefficients.resize(coefSize);
    bakedCoef.read((char*)&barricadeBakedCoefficients[0], coefSize);

    bakedCoef.close();

    //////////////////////////////////////////////////////////////////////////
    // Put the baked coefficients into textures
    //////////////////////////////////////////////////////////////////////////
    Texture2D* tankCoefficients = Renderer::GetInstance()->CreateTexture2D();
    tankCoefficients->SetWidth(19);
    tankCoefficients->SetHeight(1163);
    tankCoefficients->SetFilterMode(FILTER_MODE_NEAREST);
    tankCoefficients->SetTextureFormat(TEXTURE_FORMAT_RGBA32F);
    if (!tankCoefficients->Create()) {
        Logger::GetInstance()->Error("Couldn't create tank coefficients texture");
        delete tankCoefficients;
        return 1;
    }

    if (!tankCoefficients->SetPixelDataFloats((float*)&tankBakedCoefficients[0], 19, 1163)) {
        Logger::GetInstance()->Error("Couldn't set the baked coefficients for the tank texture");
        delete tankCoefficients;
        return 1;
    }

    Texture2D* groundCoefficients = Renderer::GetInstance()->CreateTexture2D();
    groundCoefficients->SetWidth(16);
    groundCoefficients->SetHeight(257);
    groundCoefficients->SetFilterMode(FILTER_MODE_NEAREST);
    groundCoefficients->SetTextureFormat(TEXTURE_FORMAT_RGBA32F);
    if (!groundCoefficients->Create()) {
        Logger::GetInstance()->Error("Couldn't create ground coefficients texture");
        delete tankCoefficients;
        delete groundCoefficients;
        return 1;
    }

    if (!groundCoefficients->SetPixelDataFloats((float*)&groundBakedCoefficients[0], 16, 257)) {
        Logger::GetInstance()->Error("Couldn't set the baked coefficients for the ground texture");
        delete tankCoefficients;
        delete groundCoefficients;
        return 1;
    }

    Texture2D* barricadeCoefficients = Renderer::GetInstance()->CreateTexture2D();
    barricadeCoefficients->SetWidth(32);
    barricadeCoefficients->SetHeight(305);
    barricadeCoefficients->SetFilterMode(FILTER_MODE_NEAREST);
    barricadeCoefficients->SetTextureFormat(TEXTURE_FORMAT_RGBA32F);
    if (!barricadeCoefficients->Create()) {
        Logger::GetInstance()->Error("Couldn't create barricade coefficients texture");
        delete tankCoefficients;
        delete groundCoefficients;
        delete barricadeCoefficients;
        return 1;
    }

    if (!barricadeCoefficients->SetPixelDataFloats((float*)&barricadeBakedCoefficients[0], 32, 305)) {
        Logger::GetInstance()->Error("Couldn't set the baked coefficients for the barricade texture");
        delete tankCoefficients;
        delete groundCoefficients;
        delete barricadeCoefficients;
        return 1;
    }

    tankMaterial.SetUniformTexture("baked_coeffs", tankCoefficients);
    groundMaterial.SetUniformTexture("baked_coeffs", groundCoefficients);
    barricadeMaterial.SetUniformTexture("baked_coeffs", barricadeCoefficients);
    //////////////////////////////////////////////////////////////////////////
    // Misc stuff
    //////////////////////////////////////////////////////////////////////////
    Vector3 cameraPosition(3.5f, 2.5f, -12.5f);
    Renderer::GetInstance()->CameraLookAt(cameraPosition, -Vector3::LOOK);
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

    float aspectRatio = (float)Renderer::GetInstance()->GetScreenWidth() / (float)Renderer::GetInstance()->GetScreenHeight();
    Vector3 sunPosition(10.0f, 15.0f, 15.0f);

    Matrix4x4 biasMatrix(0.5f, 0.0f, 0.0f, 0.5f,
                         0.0f, 0.5f, 0.0f, 0.5f,
                         0.0f, 0.0f, 0.5f, 0.5f,
                         0.0f, 0.0f, 0.0f, 1.0f);

    while (window.IsOpen()) {
        begin = clock();

        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

        //////////////////////////////////////////////////////////////////////////
        // Render the scene from the point of view of the sun
        //////////////////////////////////////////////////////////////////////////
        Renderer::GetInstance()->CameraLookAt(sunPosition, Vector3::ZERO);
        Renderer::GetInstance()->PerspectiveProjection(75.0f, 1.0f, 1.0f, 100.0f);
        Matrix4x4 shadowMatrix = Renderer::GetInstance()->GetViewProjectionMatrix();
        renderTexture->Bind();

        tankNode.SetMaterial(&recordDepthMaterial);
        barricadeNode.SetMaterial(&recordDepthMaterial);

        Renderer::GetInstance()->SetCullingMethod(CULLING_METHOD_FRONT_FACE);
        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->StartRender();
        
        tankNode.Render();
        barricadeNode.Render();

        Renderer::GetInstance()->EndRender();

        //////////////////////////////////////////////////////////////////////////
        // Render from the camera's perspective
        //////////////////////////////////////////////////////////////////////////
        Renderer::GetInstance()->SetCullingMethod(CULLING_METHOD_BACK_FACE);
        Renderer::GetInstance()->CameraLookAt(cameraPosition, Vector3::ZERO);
        Renderer::GetInstance()->PerspectiveProjection(45.0f, aspectRatio, 1.0f, 100.0f);

        tankNode.SetMaterial(&tankMaterial);
        barricadeNode.SetMaterial(&groundMaterial);

        Renderer::GetInstance()->BindShader(shader);
        shader->SetUniformMatrix4x4("shadowMatrix", biasMatrix * shadowMatrix);

        hdrRenderTexture->Bind();
        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->StartRender();

        Renderer::GetInstance()->EnableDepthWrite(false);
        skyboxNode.Render();
        Renderer::GetInstance()->EnableDepthWrite(true);

        shader->SetUniformVector2("baked_size", 19.0f, 1163.0f);
        tankNode.Render();

        shader->SetUniformVector2("baked_size", 16.0f, 257.0f);
        groundNode.Render();

        shader->SetUniformVector2("baked_size", 32.0f, 305.0f);
        barricadeNode.Render();

        Renderer::GetInstance()->EndRender();

        //////////////////////////////////////////////////////////////////////////
        // HDR process
        //////////////////////////////////////////////////////////////////////////
        // Down sample
        downSampleRenderTexture->Bind();
        fullscreenQuadNode.SetMaterial(&downSampleMaterial);

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->StartRender();
        fullscreenQuadNode.Render();
        Renderer::GetInstance()->EndRender();

        // Horizontal blur
        horizontalBlurRenderTexture->Bind();
        fullscreenQuadNode.SetMaterial(&blurXMaterial);

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->StartRender();
        fullscreenQuadNode.Render();
        Renderer::GetInstance()->EndRender();

        // Vertical blur
        verticalBlurRenderTexture->Bind();
        fullscreenQuadNode.SetMaterial(&blurYMaterial);

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->StartRender();
        fullscreenQuadNode.Render();
        Renderer::GetInstance()->EndRender();

        // Final composition of the image with tone mapping
        fullscreenQuadNode.SetMaterial(&toneMappingMaterial);
        Renderer::GetInstance()->BindScreenBuffer();
        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->StartRender();

        fullscreenQuadNode.Render();

        Text::GetInstance()->Write("FPS: " + to_string(fps), 5, 5);

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
    
    delete tankCoefficients;
    delete groundCoefficients;
    delete barricadeCoefficients;

    return 0;
}
