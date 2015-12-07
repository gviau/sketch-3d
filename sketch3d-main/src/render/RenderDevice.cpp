#include "render/RenderDevice.h"

#include "render/OpenGL/RenderContextOpenGL.h"
#include "render/OpenGL/RenderDeviceOpenGL.h"

#if PLATFORM == PLATFORM_WIN32
#include "render/Direct3D11/RenderContextDirect3D11.h"
#include "render/Direct3D11/RenderDeviceDirect3D11.h"
#endif

#include "system/Logger.h"

namespace Sketch3D {
RenderDevice::~RenderDevice() {
}

 bool SKETCH_3D_API CreateRenderDevice(const shared_ptr<RenderContext>& renderContext, shared_ptr<RenderDevice>& renderDevice) {
    // If we are on a Unix system, we can only create an opengl render context
    RenderDevice* pRenderDevice = nullptr;
    RenderContext* pRenderContext = renderContext.get();
    if (pRenderContext == nullptr)
    {
        Logger::GetInstance()->Error("Null render context passed to CreateRenderDevice");
        return false;
    }

#if PLATFORM == PLATFORM_WIN32
    if (dynamic_cast<RenderContextDirect3D11*>(pRenderContext) != nullptr)
    {
        renderDevice.reset(new RenderDeviceDirect3D11);
        return true;
    }
    else
    {
        renderDevice.reset(new RenderDeviceOpenGL);
        return true;
    }
#elif PLATFORM == PLATFORM_LINUX
    renderDevice.reset(RenderDeviceOpenGL);
    return true;
#endif

    return false;
}
}