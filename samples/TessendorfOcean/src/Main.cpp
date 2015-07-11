#include "Ocean.h"

#include <render/Material.h>
#include <render/Renderer.h>

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
    renderParameters.displayFormat = DISPLAY_FORMAT_X8R8G8B8;
    renderParameters.refreshRate = 0;
    renderParameters.depthStencilBits = DEPTH_STENCIL_BITS_D24X8;

    Renderer::GetInstance()->Initialize(RenderSystem_t::OPENGL, window, renderParameters);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    Ocean ocean(128, 0.001f, Vector2(5.0f, 3.0f), 32.0f);

    double t = 0.0;
    clock_t begin, end;

    Renderer::GetInstance()->CameraLookAt(Vector3(10.0f, 15.0f, -35.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3::UP);

    while (window.IsOpen()) {
        begin = clock();

        WindowEvent windowEvent;
        while (window.PollEvents(windowEvent)) {
        }

        ocean.EvaluateWaves(t);
        ocean.PrepareForRender();

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->Render();
        Renderer::GetInstance()->EndRender();

        Renderer::GetInstance()->PresentFrame();

        end = clock();
        t += double(end - begin) / CLOCKS_PER_SEC;
    }

    return 0;
}
