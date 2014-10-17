#include "Ocean.h"

#include <render/Material.h>
#include <render/Renderer.h>

#include <system/Logger.h>
#include <system/Window.h>
using namespace Sketch3D;

#include <Windows.h>

#include <time.h>
using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    Window window("Sample_Ocean", 1024, 768, true);
    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);

    Ocean ocean(128, 0.0001f, Vector2(5.0f, 3.0f), 64.0f);

    double t = 0.0;
    clock_t begin, end;

    Renderer::GetInstance()->CameraLookAt(Vector3(10.0f, 5.0f, 10.0f), Vector3(-32.0f, 1.0f, -32.0f), Vector3::UP);

    MSG msg;
    while (window.IsOpen()) {
        begin = clock();

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        ocean.EvaluateWaves(t);
        ocean.PrepareForRender();

        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->Render();
        Renderer::GetInstance()->EndRender();

        end = clock();
        t += double(end - begin) / CLOCKS_PER_SEC;
    }

    return msg.wParam;
}