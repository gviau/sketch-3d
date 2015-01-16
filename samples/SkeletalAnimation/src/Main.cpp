#include <math/Quaternion.h>

#include <render/Material.h>
#include <render/Node.h>
#include <render/Renderer.h>
#include <render/Shader.h>
#include <render/SkinnedMesh.h>

#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <chrono>
#include <string>
#include <vector>
using namespace std;

#if defined(__WIN32__) || defined(_WIN32)
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif

    // CHANGE THIS VALUE TO ANIMATE ON CPU OR GPU
    bool isSkinnedOnGpu = true;

    Window window("Sample_SkeletalAnimation", 1024, 768, true);
    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    // Create the skinned mesh
    VertexAttributesMap_t vertexAttributes;
    vertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
    vertexAttributes[VERTEX_ATTRIBUTES_NORMAL] = 1;
    vertexAttributes[VERTEX_ATTRIBUTES_TEX_COORDS] = 2;

    if (isSkinnedOnGpu) {
        vertexAttributes[VERTEX_ATTRIBUTES_BONES] = 3;
        vertexAttributes[VERTEX_ATTRIBUTES_WEIGHTS] = 4;
    }
    SkinnedMesh mesh("Media/Bob.md5mesh", vertexAttributes, isSkinnedOnGpu);

    // Create the material
    string vertexFilename = "";
    if (isSkinnedOnGpu) {
        vertexFilename = "Shaders/SkeletalAnimation/vertSkinned.glsl";
    } else {
        vertexFilename = "Shaders/SkeletalAnimation/vert.glsl";
    }
    Shader* shader = Renderer::GetInstance()->CreateShader(vertexFilename, "Shaders/SkeletalAnimation/frag.glsl");

    Material material(shader);

    // Create the node
    Node node;
    node.SetMaterial(&material);
    node.SetMesh(&mesh);
    Renderer::GetInstance()->GetSceneTree().AddNode(&node);

    Renderer::GetInstance()->CameraLookAt(Vector3(0.0f, 35.0f, -100.0f), Vector3(0.0f, 35.0f, 1.0f));

    typedef chrono::high_resolution_clock Clock;
    chrono::system_clock::time_point begin, end;
    chrono::duration<double> deltaTime;

    mesh.SetAnimationState("Default");
    mesh.SetAnimationLoop(true);

    size_t numberOfFrames = 0;
    while (window.IsOpen()) {
        begin = Clock::now();

        WindowEvent windowEvent;
        if (window.PollEvents(windowEvent)) {
        }

        Quaternion rotationZ, rotationX;
        rotationZ.MakeFromAngleAxis(PI, Vector3::LOOK);
        rotationX.MakeFromAngleAxis(PI_OVER_2, Vector3::RIGHT);
        node.SetOrientation(rotationZ * rotationX);

        // CPU skinning
        if (!isSkinnedOnGpu) {
            if (numberOfFrames % 12 == 0) {
                mesh.Animate(deltaTime.count());
                deltaTime = chrono::duration<double>::zero();
            }
        } else {
            vector<Matrix4x4> boneTransformationMatrices;
            if (mesh.Animate(deltaTime.count(), boneTransformationMatrices)) {
                // Send the matrices to the GPU
                shader->SetUniformMatrix4x4Array("boneTransformationMatrices", &boneTransformationMatrices[0], boneTransformationMatrices.size());
            }

            deltaTime = chrono::duration<double>::zero();
        }

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->Render();
        Renderer::GetInstance()->EndRender();

        end = Clock::now();
        deltaTime += chrono::duration_cast<chrono::duration<double>>(end - begin);

        numberOfFrames += 1;
    }

    return 0;
}