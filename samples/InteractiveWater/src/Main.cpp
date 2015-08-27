#include <render/RenderContext.h>

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

    return 0;
}
