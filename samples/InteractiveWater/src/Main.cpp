#include <math/Vector4.h>

#include <render/RenderContext.h>
#include <render/RenderDevice.h>

#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <string>
#include <vector>
using namespace std;

#if PLATFORM == PLATFORM_WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
#else
int main(int argc, char** argv) {
#endif
    Window window("Test", 1024, 768, true);

    RenderParameters_t renderParameters;
    renderParameters.width = 1024;
    renderParameters.height = 768;
    renderParameters.displayFormat = DisplayFormat_t::A8R8G8B8;
    renderParameters.depthStencilBits = DepthStencilBits_t::D32;
    renderParameters.refreshRate = 0;

    shared_ptr<RenderContext> renderContext = CreateRenderContext(RenderSystem_t::OPENGL);
    bool init = renderContext->Initialize(window, renderParameters);

    shared_ptr<RenderDevice> renderDevice = CreateRenderDevice(RenderSystem_t::OPENGL);
    init = renderDevice->Initialize(renderContext);

    while (window.IsOpen())
    {
        WindowEvent event;
        if (window.PollEvents(event))
        {
        }

        renderDevice->ClearRenderTargets(Vector4(0.1f, 0.1f, 0.1f, 1.0f));

        renderContext->SwapBuffers();
    }

    return 0;
}
