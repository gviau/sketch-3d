#include "Ocean.h"

#include <render/Material.h>
#include <render/Renderer.h>

#include <system/Logger.h>
#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <Windows.h>

#include <time.h>
using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    Window window("Sample_Ocean", 1024, 768, true);
    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window);
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

        end = clock();
        t += double(end - begin) / CLOCKS_PER_SEC;
    }

    return 0;
}