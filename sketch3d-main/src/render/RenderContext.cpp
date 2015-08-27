#include "render/RenderContext.h"

#include "system/Logger.h"

#if PLATFORM == PLATFORM_WIN32
#include "render/Direct3D11/RenderContextDirect3D11.h"
#include "render/OpenGL/Win32/RenderContextOpenGLWin32.h"
#elif PLATFORM == PLATFORM_LINUX
#include "render/OpenGL/Unix/RenderContextOpenGLUnix.h"
#endif

namespace Sketch3D {
bool RenderContext::AreRenderParametersSupported(const RenderParameters_t& renderParameters) const
{
    for (size_t i = 0; i < supportedDisplayModes_.size(); i++)
    {
        const DisplayMode_t& displayMode = supportedDisplayModes_[i];

        if (displayMode.width == renderParameters.width && displayMode.height == renderParameters.height &&
            (displayMode.refreshRate == renderParameters.refreshRate || renderParameters.refreshRate == 0) &&
            displayMode.displayFormat == renderParameters.displayFormat)
        {
            return true;
        }
    }

    return false;
}

shared_ptr<RenderContext> SKETCH_3D_API CreateRenderContext(RenderSystem_t renderSystem)
{
    // If we are on a Unix system, we can only create an opengl render context
    RenderContext* renderContext = nullptr;

#if PLATFORM == PLATFORM_WIN32
    switch (renderSystem)
    {
        case RenderSystem_t::DIRECT3D11:    renderContext = new RenderContextDirect3D11; break;
        case RenderSystem_t::OPENGL:        renderContext = new RenderContextOpenGLWin32; break;
    }
#elif PLATFORM == PLATFORM_LINUX
    renderContext = new RenderContextOpenGLUnix;
#endif

    if (renderContext == nullptr)
    {
        Logger::GetInstance()->Error("Couldn't create the RenderContext : Unknown render system");
    }

    return shared_ptr<RenderContext>(renderContext);
}

}