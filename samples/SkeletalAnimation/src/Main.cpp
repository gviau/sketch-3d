#include <math/Vector4.h>

#include <framework/Camera.h>
#include <framework/RenderingPipelines/ForwardRenderingPipeline.h>
#include <framework/Light.h>
#include <framework/Scene.h>
#include <framework/SkinnedMesh.h>
#include <framework/VisualNodes/VisualNodeSkinnedMesh.h>

#include <render/RenderContext.h>
#include <render/RenderDevice.h>

#include <system/Logger.h>
#include <system/Window.h>
#include <system/WindowEvent.h>

using namespace Sketch3D;

#include <chrono>
#include <memory>
using namespace std;

#if PLATFORM == PLATFORM_WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif
	Window window("Sample_SkeletalAnimation", 1024, 768, true);

	RenderParameters_t renderParameters;
	renderParameters.width = 1024;
	renderParameters.height = 768;
	renderParameters.displayFormat = DisplayFormat_t::A8R8G8B8;
	renderParameters.depthStencilBits = DepthStencilBits_t::D32;

	shared_ptr<RenderContext> renderContext = CreateRenderContext(RenderSystem_t::DIRECT3D11);
	if (!renderContext->Initialize(window, renderParameters))
	{
		Logger::GetInstance()->Error("Couldn't initialize rendering context");
		return 1;
	}

	shared_ptr<RenderDevice> renderDevice;
	if (!CreateRenderDevice(renderContext, renderDevice))
	{
		Logger::GetInstance()->Error("Couldn't create render device!");
		return 1;
	}

	if (!renderDevice->Initialize(renderContext))
	{
		Logger::GetInstance()->Error("Couldn't initialize the rendering device");
		return 1;
	}

	ForwardRenderingPipeline forwardRenderingPipeline(renderContext, renderDevice);
	if (!forwardRenderingPipeline.Initialize())
	{
		Logger::GetInstance()->Error("Couldn't initialize the forward rendering pipeline");
		return 1;
	}

	// Load the mesh
	shared_ptr<SkinnedMesh> mesh;
	if (!LoadSkinnedMeshFromFileWithMaterial("Media/Bob.md5mesh", renderDevice, mesh, forwardRenderingPipeline.GetMaterialCodeGenerator()))
	{
		Logger::GetInstance()->Error("Couldn't load the mesh");
		return 1;
	}

	// Setup the scene
	Scene scene;
	VisualNode& rootNode = scene.GetRootNode();

	shared_ptr<VisualNodeSkinnedMesh> meshNode(new VisualNodeSkinnedMesh);
	meshNode->SetMesh(mesh);

	Quaternion rotationX, rotationY, rotationZ;
	rotationX.MakeFromAngleAxis(PI, Vector3::RIGHT);
	rotationY.MakeFromAngleAxis(PI, Vector3::UP);
	rotationZ.MakeFromAngleAxis(PI, Vector3::LOOK);
	meshNode->SetOrientation(rotationZ * rotationX * rotationY);

	rootNode.AddChild(meshNode);

	vector<shared_ptr<Light>> lights;
	
	shared_ptr<Light> light(new Light);
	light->SetPosition(Vector3(50.0f, 35.0f, 75.0f));
	light->SetLightType(LightType_t::Point);
	lights.push_back(light);

	scene.SetLights(lights);

	Camera camera;
	camera.LookAtRightHanded(Vector3(0.0f, 35.0f, 100.0f), Vector3(0.0f, 35.0f, 0.0f));

	renderDevice->CalculatePerspectiveProjectionFOVRightHanded(60.0f, 1024.0f / 768.0f, 0.01f, 1000.0f);

	mesh->SetAnimationState("Default");
	mesh->SetAnimationLoop(true);
	mesh->StartAnimation();

	clock_t begin, end;
	double deltaTime = 0.0;

	while (window.IsOpen())
	{
		begin = clock();

		WindowEvent windowEvent;
		if (window.PollEvents(windowEvent))
		{
		}

		forwardRenderingPipeline.RenderSceneFromCamera(camera, scene, deltaTime);

		end = clock();

		deltaTime = (double)(end - begin) / CLOCKS_PER_SEC;
	}

	return 0;
}