#include "Ocean.h"

#include <framework/Camera.h>

#include <render/RenderContext.h>
#include <render/RenderDevice.h>

#include <system/Logger.h>
#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <time.h>
using namespace std;

#if PLATFORM == PLATFORM_WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif
    Window window("Sample_Ocean", 1024, 768, true);
    RenderParameters_t renderParameters;
    renderParameters.width = 1024;
    renderParameters.height = 768;
    renderParameters.displayFormat = DisplayFormat_t::A8R8G8B8;
    renderParameters.refreshRate = 0;
    renderParameters.depthStencilBits = DepthStencilBits_t::D32;

    shared_ptr<RenderContext> renderContext = CreateRenderContext(RenderSystem_t::DIRECT3D11);
    renderContext->Initialize(window, renderParameters);

    shared_ptr<RenderDevice> renderDevice;
    if (!CreateRenderDevice(renderContext, renderDevice))
    {
        return 1;
    }
    renderDevice->Initialize(renderContext);

    Ocean ocean(256, 0.0001f, Vector2(5.0f, 3.0f), 128.0f, renderDevice);

    double t = 0.0;
    clock_t begin, end;

    Camera camera;
    camera.LookAtRightHanded(Vector3(10.0f, 15.0f, 35.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3::UP);

    while (window.IsOpen()) {
        begin = clock();

        WindowEvent windowEvent;
        while (window.PollEvents(windowEvent)) {
        }

        renderDevice->ClearRenderTargets(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
        renderDevice->ClearDepthStencil(true, false, 1.0f, 0);

        ocean.EvaluateWaves(t);
        ocean.Render(camera.GetViewMatrix());

        renderContext->SwapBuffers();

        end = clock();
        t += double(end - begin) / CLOCKS_PER_SEC;
    }

    return 0;
}
