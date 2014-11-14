#include <render/Material.h>
#include <render/Mesh.h>
#include <render/Renderer.h>
#include <render/Shader.h>

#include <system/Window.h>
#include <system/WindowEvent.h>
using namespace Sketch3D;

#include <string>
#include <vector>
using namespace std;

int main(int argc, char** argv) {
    Window window("Safdsaf", 1024, 768, true);
    Renderer::GetInstance()->Initialize(RENDER_SYSTEM_OPENGL, window);
    Renderer::GetInstance()->SetClearColor(0.2f, 0.2f, 0.2f);



    while (window.IsOpen()) {
        Renderer::GetInstance()->Clear();
        Renderer::GetInstance()->Render();
        Renderer::GetInstance()->EndRender();
    }

    return 0;
}
