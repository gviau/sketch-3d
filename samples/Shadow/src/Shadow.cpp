#include <render/Renderer.h>
#include <system/Window.h>
using namespace Sketch3D;

#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    Window window("Test", 1024, 768, true);
    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    MSG msg;
    while (window.IsOpen()) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Renderer::GetInstance()->BeginRender();
        Renderer::GetInstance()->Render();
        Renderer::GetInstance()->EndRender();
    }

    return msg.wParam;
}